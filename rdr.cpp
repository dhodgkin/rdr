#include <wx/wx.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>
#include <ctime>
#include <regex>
#include <map>

// Function to roll the dice
std::vector<int> roll(int roll, int die) {
    std::vector<int> rolls;
    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<int> dist(1, die);

    for (int i = 0; i < roll; ++i) {
        rolls.push_back(dist(rng));
    }

    return rolls;
}

// Function to roll dice and keep the highest or lowest rolls
std::vector<int> rollKeep(int numRolls, int die, int keep, bool keepHighest) {
    std::vector<int> rolls = roll(numRolls, die);
    std::sort(rolls.begin(), rolls.end());

    if (keepHighest) {
        return std::vector<int>(rolls.end() - keep, rolls.end());
    } else {
        return std::vector<int>(rolls.begin(), rolls.begin() + keep);
    }
}

// Function to roll dice and drop the lowest rolls
std::vector<int> rollDropLowest(int numRolls, int die, int drop) {
    std::vector<int> rolls = roll(numRolls, die);
    std::sort(rolls.begin(), rolls.end());

    // Remove the lowest 'drop' rolls
    rolls.erase(rolls.begin(), rolls.begin() + drop);

    return rolls;
}

// Function to roll dice with exploding mechanics
std::vector<int> rollExploding(int numRolls, int die) {
    std::vector<int> rolls;
    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<int> dist(1, die);

    for (int i = 0; i < numRolls; ++i) {
        int currentRoll = dist(rng);
        rolls.push_back(currentRoll);

        while (currentRoll == die) { // Explode if max value is rolled
            currentRoll = dist(rng);
            rolls.push_back(currentRoll);
        }
    }

    return rolls;
}

// Function to parse complex roll expressions
struct RollExpression {
    int numRolls;
    int dieType;
    int modifier;
    std::string operation; // For advanced operations like "kh" (keep highest)
};

std::vector<RollExpression> parseComplexInput(const std::string& input) {
    std::vector<RollExpression> expressions;
    std::regex rollRegex(R"((\d+)d(\d+)(kh\d*|kl\d*|e|dl\d*|[a-zA-Z]*)?([+-]\d+)?)");
    std::smatch match;
    std::string::const_iterator searchStart(input.cbegin());

    while (std::regex_search(searchStart, input.cend(), match, rollRegex)) {
        RollExpression expr;
        expr.numRolls = std::stoi(match[1]);
        expr.dieType = std::stoi(match[2]);
        expr.operation = match[3].matched ? match[3].str() : "";
        expr.modifier = match[4].matched ? std::stoi(match[4]) : 0;
        expressions.push_back(expr);
        searchStart = match.suffix().first;
    }

    return expressions;
}

// Main application class
class DiceRollerApp : public wxApp {
public:
    virtual bool OnInit();
};

// Main frame class
class DiceRollerFrame : public wxFrame {
public:
    DiceRollerFrame(const wxString& title);

private:
    wxTextCtrl* resultOutput;
    wxTextCtrl* advancedInput; // New input field for complex roll expressions
    wxTextCtrl* rollLog; // Added roll log as a member variable

    void OnRollDice(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(DiceRollerFrame, wxFrame)
    EVT_BUTTON(10001, DiceRollerFrame::OnRollDice)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(DiceRollerApp);

bool DiceRollerApp::OnInit() {
    DiceRollerFrame* frame = new DiceRollerFrame("Dice Roller");
    frame->Show(true);
    return true;
}

DiceRollerFrame::DiceRollerFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(400, 300)) {

    wxPanel* panel = new wxPanel(this, wxID_ANY);

    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* hbox4 = new wxBoxSizer(wxHORIZONTAL);
    hbox4->Add(new wxStaticText(panel, wxID_ANY, "Advanced Roll:"), 0, wxRIGHT, 8);
    advancedInput = new wxTextCtrl(panel, wxID_ANY);
    hbox4->Add(advancedInput, 1);
    vbox->Add(hbox4, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    wxButton* rollButton = new wxButton(panel, 10001, "Roll Dice");
    vbox->Add(rollButton, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    resultOutput = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    vbox->Add(resultOutput, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    // Added a calculator-like interface with buttons for constructing expressions
    wxGridSizer* gridSizer = new wxGridSizer(4, 5, 5, 5);

    // Add buttons for numbers 0-9
    for (int i = 1; i <= 9; ++i) {
        wxButton* numButton = new wxButton(panel, 20000 + i, std::to_string(i));
        gridSizer->Add(numButton, 0, wxEXPAND);
        Bind(wxEVT_BUTTON, [this, i](wxCommandEvent&) {
            advancedInput->AppendText(std::to_string(i));
        }, 20000 + i);
    }

    // Add button for 0
    wxButton* zeroButton = new wxButton(panel, 20010, "0");
    gridSizer->Add(zeroButton, 0, wxEXPAND);
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        advancedInput->AppendText("0");
    }, 20010);

    // Add buttons for dice notation and operators
    std::vector<std::pair<std::string, int>> buttons = {
        {"d", 20011}, {"+", 20012}, {"-", 20013}, {"kh", 20014}, {"dl", 20015}, {"e", 20016}
    };

    for (const auto& [label, id] : buttons) {
        wxButton* button = new wxButton(panel, id, label);
        gridSizer->Add(button, 0, wxEXPAND);
        Bind(wxEVT_BUTTON, [this, label](wxCommandEvent&) {
            advancedInput->AppendText(label);
        }, id);
    }

    // Add "AC" button for clearing the expression
    wxButton* acButton = new wxButton(panel, 20017, "AC");
    gridSizer->Add(acButton, 0, wxEXPAND);
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        advancedInput->Clear();
    }, 20017);

    // Add "Stat Roll" button for rolling 4d6dl1
    wxButton* statRollButton = new wxButton(panel, 20018, "Stat Roll");
    gridSizer->Add(statRollButton, 0, wxEXPAND);
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        advancedInput->SetValue("4d6dl1");
        wxCommandEvent rollEvent;
        OnRollDice(rollEvent); // Trigger the roll automatically
    }, 20018);

    vbox->Add(gridSizer, 0, wxEXPAND | wxALL, 10);

    // Add a roll log area
    wxStaticText* logLabel = new wxStaticText(panel, wxID_ANY, "Roll Log:");
    vbox->Add(logLabel, 0, wxLEFT | wxTOP, 10);
    rollLog = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    vbox->Add(rollLog, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    panel->SetSizer(vbox);
}

void DiceRollerFrame::OnRollDice(wxCommandEvent& event) {
    std::string advancedRoll = std::string(advancedInput->GetValue().mb_str());

    if (!advancedRoll.empty()) {
        // Parse the advanced roll input
        std::vector<RollExpression> expressions = parseComplexInput(advancedRoll);
        std::ostringstream result;

        for (const auto& expr : expressions) {
            std::vector<int> rolls;

            // Extract numeric parameter from operation (e.g., "kh3" -> 3)
            int param = 0;
            if (expr.operation.length() > 2) {
                param = std::stoi(expr.operation.substr(2));
            }

            if (expr.operation.rfind("kh", 0) == 0) { // Keep Highest
                rolls = rollKeep(expr.numRolls, expr.dieType, param > 0 ? param : expr.numRolls, true);
            } else if (expr.operation.rfind("kl", 0) == 0) { // Keep Lowest
                rolls = rollKeep(expr.numRolls, expr.dieType, param > 0 ? param : expr.numRolls, false);
            } else if (expr.operation.rfind("dl", 0) == 0) { // Drop Lowest
                rolls = rollDropLowest(expr.numRolls, expr.dieType, param > 0 ? param : 1);
            } else if (expr.operation == "e") { // Exploding Dice
                rolls = rollExploding(expr.numRolls, expr.dieType);
            } else { // Standard
                rolls = roll(expr.numRolls, expr.dieType);
            }

            int sum = 0;
            for (int r : rolls) {
                sum += r;
            }

            // Special handling for stat rolls (4d6dl1)
            if (advancedRoll == "4d6dl1") {
                result << "Stat Roll - Kept Rolls: [";
                for (size_t i = 0; i < rolls.size(); ++i) {
                    result << rolls[i];
                    if (i < rolls.size() - 1) {
                        result << ", ";
                    }
                }
                result << "]\n";
                result << "Final: " << sum << "\n\n";
            } else {
                result << expr.numRolls << "d" << expr.dieType;
                if (expr.modifier != 0) {
                    result << (expr.modifier > 0 ? "+" : "") << expr.modifier;
                }
                result << " Rolls: [";
                for (size_t i = 0; i < rolls.size(); ++i) {
                    result << rolls[i];
                    if (i < rolls.size() - 1) {
                        result << ", ";
                    }
                }
                result << "]\n";

                // Show each roll in detail
                result << "Detailed Rolls:\n";
                for (size_t i = 0; i < rolls.size(); ++i) {
                    result << "Roll " << (i + 1) << ": " << rolls[i] << "\n";
                }

                if (expr.modifier == 0) {
                    result << "Final: " << sum << "\n\n";
                } else {
                    result << "Final: " << sum << " + " << expr.modifier << " = " << (sum + expr.modifier) << "\n\n";
                }
            }
        }

        resultOutput->SetValue(result.str());

        // Append the result to the roll log
        rollLog->AppendText(result.str() + "\n");
    } else {
        resultOutput->SetValue("Please enter a valid advanced roll expression.");
    }
}