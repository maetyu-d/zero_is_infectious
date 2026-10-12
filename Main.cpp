#include <juce_gui_basics/juce_gui_basics.h>
#include "Engine.h"

using namespace juce;

namespace ui
{
const Colour paper { 0xfffaf9f5 }, white { 0xfffffdf9 }, ink { 0xff2e3930 };
const Colour sub { 0xff455348 }, muted { 0xff536257 }, rule { 0xffa4b2a2 };
const Colour green { 0xff3e6749 }, coral { 0xff964c38 }, blue { 0xff386588 };
const std::array<Colour, 6> accents {{ Colour(0xff3f8077), Colour(0xff557b9e),
    Colour(0xffad6881), Colour(0xff8875a2), Colour(0xffad7951), Colour(0xff557c5b) }};
const std::array<String, 6> names {{ "Transfer", "Exchange", "Invert", "Sever", "Sacrifice", "Anchor" }};
String utf(const char* s) { return String::fromUTF8(s); }
Font font(float size, bool bold = false)
{
    return Font(FontOptions("Helvetica Neue", size, bold ? Font::bold : Font::plain));
}
void text(Graphics& g, const String& s, Rectangle<float> r, float size = 12,
          Colour colour = sub, Justification align = Justification::centredLeft)
{
    g.setColour(colour);
    g.setFont(font(std::max(size, 12.f), size < 14));
    g.drawText(s, r, align, false);
}
void caption(Graphics& g, const String& s, Rectangle<float> r, Colour colour = sub)
{
    g.setColour(colour);
    g.setFont(font(11.5f, true).withExtraKerningFactor(.035f));
    g.drawText(s, r, Justification::centredLeft, false);
}
void paragraph(Graphics& g, const String& s, Rectangle<float> r, float size = 12,
               Colour colour = sub, int lines = 3)
{
    g.setColour(colour);
    g.setFont(font(std::max(size, 13.f)));
    g.drawFittedText(s, r.toNearestInt(), Justification::topLeft, lines, 1.0f);
}
void ruleLine(Graphics& g, float x, float y, float width)
{
    g.setColour(rule);
    g.fillRect(x, float(roundToInt(y)), width, 1.5f);
}
void icon(Graphics& g, int op, Rectangle<float> r, Colour colour)
{
    g.setColour(colour);
    Path p;
    switch (op)
    {
        case 0: p.startNewSubPath(2, 12); p.lineTo(21, 12); p.lineTo(16, 7);
                p.startNewSubPath(21, 12); p.lineTo(16, 17); break;
        case 1: p.startNewSubPath(2, 6); p.lineTo(21, 6); p.lineTo(16, 1);
                p.startNewSubPath(22, 18); p.lineTo(3, 18); p.lineTo(8, 23); break;
        case 2: p.startNewSubPath(12, 2); p.lineTo(12, 13);
                p.startNewSubPath(7, 7); p.lineTo(17, 7);
                p.startNewSubPath(7, 20); p.lineTo(17, 20); break;
        case 3: p.startNewSubPath(2, 12); p.lineTo(11, 12);
                p.startNewSubPath(14, 3); p.lineTo(14, 21);
                p.startNewSubPath(18, 12); p.lineTo(22, 12); break;
        case 4: p.startNewSubPath(5, 5); p.lineTo(19, 19);
                p.startNewSubPath(19, 5); p.lineTo(5, 19); break;
        case 5: p.addEllipse(3, 3, 18, 18); p.addEllipse(9, 9, 6, 6); break;
    }
    p.applyTransform(AffineTransform::scale(r.getWidth() / 24).translated(r.getX(), r.getY()));
    g.strokePath(p, PathStrokeType(1.9f));
}

// Real JUCE buttons provide keyboard focus, hover feedback, and accessibility names.
class Button : public juce::Button
{
public:
    enum Style { plain, primary, bordered };
    Style style = plain;
    String label, shortcut;
    int operation = -1;
    bool corrupted = false, quiet = false;
    Button(const String& name) : juce::Button(name), label(name) {}
    void paintButton(Graphics& g, bool over, bool down) override
    {
        auto r = getLocalBounds().toFloat();
        auto colour = operation >= 0 ? accents[operation] : green;
        bool chosen = getToggleState();
        float alpha = isEnabled() ? 1.0f : .38f;
        auto fill = style == primary ? (over ? Colour(0xff48624d) : ink)
                    : quiet && !over && !down ? Colours::transparentBlack
                    : chosen ? colour.withAlpha(.10f)
                    : over || down ? colour.withAlpha(.055f) : white.withAlpha(.65f);
        g.setColour(fill.withMultipliedAlpha(alpha));
        g.fillRoundedRectangle(r.reduced(.5f), 4);
        if (style == bordered || operation >= 0 || (!quiet && chosen) || over)
        {
            g.setColour((chosen ? colour : over ? muted : rule).withMultipliedAlpha(alpha));
            g.drawRoundedRectangle(r.reduced(.75f), 4, 1.5f);
        }
        if (operation >= 0)
        {
            if (chosen)
            {
                g.setColour(colour.withMultipliedAlpha(alpha));
                g.fillRoundedRectangle(1, 9, 2, r.getHeight() - 18, 1);
            }
            icon(g, operation, { 12, down ? 11.f : 10.f, 22, 22 }, colour.withMultipliedAlpha(alpha));
            Rectangle<float> key(r.getWidth() - 29, 10, 18, 18);
            g.setColour(rule.withMultipliedAlpha(alpha)); g.drawRoundedRectangle(key, 3, 1.25f);
            ui::text(g, String(operation + 1), key, 10, muted.withMultipliedAlpha(alpha), Justification::centred);
            if (operation == 4)
                ui::text(g, "2 AP", { r.getWidth() - 62, 11, 28, 17 }, 8.5f, colour.withMultipliedAlpha(alpha));
            ui::text(g, label, { 12, r.getHeight() - 29, r.getWidth() - 18, 21 }, 12,
                 ink.withMultipliedAlpha(alpha));
            if (chosen) { g.setColour(colour); g.fillEllipse(r.getWidth() - 12, r.getHeight() - 11, 4, 4); }
            if (corrupted) { g.setColour(coral); g.fillEllipse(r.getWidth() - 40, 17, 4, 4); }
        }
        else
        {
            ui::text(g, label, r.reduced(label.length() == 1 ? 3.f : quiet ? 6.f : 12.f, 0.f), 12,
                 (style == primary ? white : quiet && chosen ? green : ink).withMultipliedAlpha(alpha),
                 shortcut.isEmpty() ? Justification::centred : Justification::centredLeft);
            if (shortcut.isNotEmpty())
                ui::text(g, shortcut, r.reduced(12, 0), 10,
                     (style == primary ? white.withAlpha(.9f) : muted).withMultipliedAlpha(alpha),
                     Justification::centredRight);
        }
        if (hasKeyboardFocus(false))
        {
            g.setColour(green);
            g.drawRoundedRectangle(r.reduced(2), 2, 1.5f);
        }
    }
};

class Canvas : public Component
{
public:
    std::function<void(Graphics&)> draw;
    void paint(Graphics& g) override { if (draw) draw(g); }
};

class Look : public LookAndFeel_V4
{
public:
    Look()
    {
        setColour(ComboBox::backgroundColourId, white);
        setColour(ComboBox::textColourId, ink);
        setColour(ComboBox::outlineColourId, rule);
        setColour(ComboBox::arrowColourId, sub);
        setColour(PopupMenu::backgroundColourId, white);
        setColour(PopupMenu::textColourId, ink);
        setColour(PopupMenu::highlightedBackgroundColourId, Colour(0xffe6edde));
        setColour(PopupMenu::highlightedTextColourId, ink);
        setColour(ToggleButton::textColourId, ink);
        setColour(ToggleButton::tickColourId, green);
        setColour(ToggleButton::tickDisabledColourId, rule);
    }
    Font getComboBoxFont(ComboBox&) override { return font(14); }
    Font getPopupMenuFont() override { return font(13); }
    void drawToggleButton(Graphics& g, ToggleButton& b, bool over, bool) override
    {
        Rectangle<float> box(0, (b.getHeight() - 18.f) / 2, 18, 18);
        g.setColour(b.getToggleState() ? green : white);
        g.fillRoundedRectangle(box, 2);
        g.setColour(over ? green : rule);
        g.drawRoundedRectangle(box, 2, 1.5f);
        if (b.getToggleState())
        {
            Path p; p.startNewSubPath(4, box.getY() + 9); p.lineTo(8, box.getY() + 13);
            p.lineTo(14, box.getY() + 5);
            g.setColour(white); g.strokePath(p, PathStrokeType(1.6f));
        }
        text(g, b.getButtonText(), { 28, 0, b.getWidth() - 28.f, float(b.getHeight()) }, 14, ink);
        if (b.hasKeyboardFocus(false)) { g.setColour(green); g.drawRect(b.getLocalBounds(), 1); }
    }
};
}

class Board : public Component
{
    zero::Game game;
    std::vector<zero::Game> undo;
    int operation = 5, selected = -1, hovered = -1;
    enum Panel { none, setupPanel, helpPanel, resultPanel } panel = setupPanel;
    Panel helpReturn = none;
    bool preview = true, expanded = false, historyOpen = false;
    String message;
    ui::Look look;
    ui::Canvas sidebarCanvas;
    Viewport sidebarViewport;
    ComboBox sizeBox, difficultyBox;
    ToggleButton dynamic { "Unstable geometry" }, worldSize { "Zero shapes the world" };
    ui::Button reset { "Reset" }, newWorld { "New world" }, setupButton { "Setup" }, help { "Rules" };
    ui::Button end { "End turn" }, reverseButton { "Undo" }, previewButton { "Preview" };
    ui::Button detailButton { "Details" }, historyButton { "History" };
    ui::Button modalPrimary { "Begin" }, modalSecondary { "Rules" }, modalClose { "Close" };
    std::array<std::unique_ptr<ui::Button>, 6> tools;
    Rectangle<float> board, rail, stats, boardHeading, toolInfo, forecast, timeline, objective;
    Rectangle<float> outcome, status, footer, dialog;
    float top = 0;
    bool compact = false;

    String value(int v) const
    {
        return v == zero::Void ? "Void" : v > 0 ? "+" + String(v)
             : v < 0 ? ui::utf("−") + String(-v) : "0";
    }
    String cellName(int i) const { return "R" + String(i / game.size + 1) + " C" + String(i % game.size + 1); }
    bool multi() const { return operation == 0 || operation == 1 || operation == 3; }
    bool usable(int op) const { return !game.phase && game.ap >= (op == 4 ? 2 : 1); }
    float gap() const { return game.size >= 11 ? 2.f : 3.f; }
    float stepX() const { return (board.getWidth() + gap()) / game.size; }
    float stepY() const { return (board.getHeight() + gap()) / game.size; }
    Rectangle<float> cellRect(int i) const
    {
        return { board.getX() + (i % game.size) * stepX(), board.getY() + (i / game.size) * stepY(),
                 stepX() - gap(), stepY() - gap() };
    }
    int hitCell(Point<float> p) const
    {
        if (!board.contains(p)) return -1;
        auto i = int((p.x - board.getX()) / stepX()) + game.size * int((p.y - board.getY()) / stepY());
        return i < int(game.cells.size()) && cellRect(i).contains(p) ? i : -1;
    }
    String toolDescription() const
    {
        if (!game.phase && game.ap == 0) return "No actions left. End the turn to resolve the pulse.";
        if (selected >= 0)
            return cellName(selected) + (operation == 0 ? " selected. Choose a recipient within two cells."
                                                       : " selected. Choose an adjacent cell.");
        switch (operation)
        {
            case 0: return game.mutated(0) ? "Donor loses 2; recipient gains 1. Select a donor, then a recipient."
                                          : "Move 1 magnitude within two cells. Select a donor, then a recipient.";
            case 1: return "Swap neighbouring values. Select one cell, then its neighbour.";
            case 2: return game.mutated(2) ? ui::utf("Reverse a sign. Values of +1 or −1 become zero.")
                                          : "Reverse the sign of a nonzero value. Select one cell.";
            case 3: return game.mutated(3) ? "Block an adjacent edge for one pulse. Select both edge cells."
                                          : "Block an adjacent edge for two pulses. Select both edge cells.";
            case 4: return "Remove a cell permanently. Costs two actions. Select one cell.";
            default: return "Select a zero to stop its spread for " + String(game.difficulty == 2 ? 2 : 3) + " pulses.";
        }
    }
    void update()
    {
        for (int i = 0; i < 6; ++i)
        {
            tools[i]->setToggleState(i == operation, dontSendNotification);
            tools[i]->setEnabled(usable(i));
            tools[i]->corrupted = game.mutated(i);
            tools[i]->setTooltip(ui::names[i] + ": " + String(i == 4 ? 2 : 1) + " action(s). Key " + String(i + 1));
            tools[i]->repaint();
        }
        end.setEnabled(!game.phase);
        reverseButton.setEnabled(!game.phase && !undo.empty());
        previewButton.label = preview ? "Preview on" : "Preview off";
        previewButton.setToggleState(preview, dontSendNotification);
        detailButton.label = expanded ? "Hide details" : "Details";
        historyButton.label = historyOpen ? "Close history" : "History";
        for (auto* button : { &reset, &newWorld, &setupButton, &help, &end, &reverseButton,
                              &previewButton, &detailButton, &historyButton, &modalPrimary, &modalSecondary })
        { button->setButtonText(button->label); button->setName(button->label); }
        sidebarCanvas.repaint();
        repaint();
    }
    void setPanel(Panel p)
    {
        panel = p;
        bool base = p == none;
        for (auto* c : { &reset, &newWorld, &setupButton, &help, &end, &reverseButton,
                         &previewButton, &detailButton, &historyButton }) c->setVisible(base);
        for (auto& b : tools) b->setVisible(base);
        sidebarViewport.setVisible(base);
        sizeBox.setVisible(p == setupPanel); difficultyBox.setVisible(p == setupPanel);
        dynamic.setVisible(p == setupPanel); worldSize.setVisible(p == setupPanel);
        modalPrimary.setVisible(!base); modalSecondary.setVisible(p != none && p != helpPanel);
        modalClose.setVisible(!base);
        modalPrimary.label = p == setupPanel ? "Begin" : p == helpPanel ? "Return" : "Retry";
        modalPrimary.shortcut = ui::utf("↗");
        modalSecondary.label = p == resultPanel ? "New world" : "Rules";
        resized(); update();
        if (base) grabKeyboardFocus();
        else if (p == setupPanel) sizeBox.grabKeyboardFocus();
        else modalPrimary.grabKeyboardFocus();
    }
    void start(uint32_t seed)
    {
        sidebarViewport.setViewPosition(0, 0);
        game = zero::Game(seed, 5 + 2 * (sizeBox.getSelectedId() - 1), difficultyBox.getSelectedId() - 1,
                         dynamic.getToggleState(), worldSize.getToggleState());
        selected = hovered = -1; operation = 5; undo.clear(); message.clear(); historyOpen = false;
        setPanel(none);
    }
    uint32_t freshSeed() const { return uint32_t(Random::getSystemRandom().nextInt(0xffffff)) + 1000; }
    void openSetup()
    {
        sizeBox.setSelectedId((game.initial - 5) / 2 + 1, dontSendNotification);
        difficultyBox.setSelectedId(game.difficulty + 1, dontSendNotification);
        dynamic.setToggleState(game.dynamic, dontSendNotification);
        worldSize.setToggleState(game.worldSize, dontSendNotification);
        setPanel(setupPanel);
    }
    void openHelp() { helpReturn = panel; setPanel(helpPanel); }
    void closePanel() { setPanel(panel == helpPanel ? helpReturn : none); }
    void chooseTool(int i)
    {
        if (!usable(i)) return;
        operation = i; selected = -1; message.clear(); update(); grabKeyboardFocus();
    }
    void clickCell(int i)
    {
        if (i < 0 || i >= int(game.cells.size()) || !usable(operation)) return;
        if (multi() && selected < 0)
        {
            if (game.cells[i].v == zero::Void) message = "That cell is absent.";
            else if (operation == 0 && (game.cells[i].v == 0 || std::abs(game.cells[i].v) < 2))
                message = "Choose a donor with magnitude 2 or more.";
            else { selected = i; message.clear(); }
            update(); return;
        }
        if (multi() && selected == i) { selected = -1; message.clear(); update(); return; }
        auto before = game;
        if (game.perform(operation, i, selected))
        {
            undo.push_back(before);
            if (undo.size() > 3) undo.erase(undo.begin());
            game.log.insert(game.log.begin(), ui::names[operation].toStdString() + " " + cellName(i).toStdString());
            if (game.log.size() > 6) game.log.resize(6);
            selected = -1; message.clear();
        }
        else message = operation == 5 ? "Only a zero can be anchored." : "Choose a valid target.";
        update();
    }
    void nextTurn()
    {
        if (game.phase) return;
        game.resolve(); undo.clear(); selected = hovered = -1; message.clear();
        if (game.phase) setPanel(resultPanel);
        update();
    }
    void reverse()
    {
        if (undo.empty() || game.phase) return;
        game = undo.back(); undo.pop_back(); selected = -1; message = "Action reversed."; update();
    }

public:
    Board()
    {
        setLookAndFeel(&look); setWantsKeyboardFocus(true);
        const char* profiles[] = { "Containment", "Tactics", "Strategy", "Fronts", "Ecology" };
        for (int i = 0; i < 5; ++i)
            sizeBox.addItem(String(5 + i * 2) + ui::utf(" × ") + String(5 + i * 2) + "  /  " + profiles[i], i + 1);
        difficultyBox.addItem("Easy", 1); difficultyBox.addItem("Normal", 2); difficultyBox.addItem("Hard", 3);
        sizeBox.setSelectedId(3); difficultyBox.setSelectedId(2);
        sizeBox.setName("Starting board size"); difficultyBox.setName("Difficulty");
        for (auto* c : { static_cast<Component*>(&sizeBox), static_cast<Component*>(&difficultyBox),
                        static_cast<Component*>(&dynamic), static_cast<Component*>(&worldSize) }) addAndMakeVisible(c);
        for (auto* c : { &sizeBox, &difficultyBox }) c->onChange = [this] { repaint(); };
        for (auto* b : { &reset, &newWorld, &setupButton, &help, &end, &reverseButton,
                         &previewButton, &detailButton, &historyButton, &modalPrimary, &modalSecondary, &modalClose })
            addAndMakeVisible(b);
        for (int i = 0; i < 6; ++i)
        {
            tools[i] = std::make_unique<ui::Button>(ui::names[i]);
            tools[i]->operation = i; tools[i]->onClick = [this, i] { chooseTool(i); };
            addAndMakeVisible(*tools[i]);
        }
        sidebarViewport.setViewedComponent(&sidebarCanvas, false);
        sidebarViewport.setScrollBarsShown(true, false);
        sidebarViewport.setScrollBarThickness(6);
        sidebarViewport.getVerticalScrollBar().setColour(ScrollBar::thumbColourId, ui::muted.withAlpha(.4f));
        sidebarViewport.getVerticalScrollBar().setColour(ScrollBar::backgroundColourId, Colours::transparentBlack);
        sidebarCanvas.draw = [this](Graphics& g)
        {
            g.addTransform(AffineTransform::translation(-rail.getX(), -board.getY()));
            auto result = game;
            if (!game.phase) result.resolve();
            paintSidebar(g, result);
        };
        addAndMakeVisible(sidebarViewport);
        for (auto& b : tools) sidebarCanvas.addAndMakeVisible(*b);
        sidebarCanvas.addAndMakeVisible(detailButton);
        reset.onClick = [this] { start(game.seed); };
        newWorld.onClick = [this] { start(freshSeed()); };
        setupButton.onClick = [this] { openSetup(); };
        help.onClick = [this] { openHelp(); };
        end.style = ui::Button::primary; end.shortcut = "Enter";
        end.onClick = [this] { nextTurn(); };
        reverseButton.style = ui::Button::bordered; reverseButton.shortcut = "Z";
        reverseButton.onClick = [this] { reverse(); };
        previewButton.onClick = [this] { preview = !preview; update(); };
        detailButton.onClick = [this] { expanded = !expanded; resized(); update(); };
        historyButton.shortcut = "+";
        historyButton.onClick = [this] { historyOpen = !historyOpen; update(); };
        modalPrimary.style = ui::Button::primary;
        modalPrimary.addShortcut(KeyPress(KeyPress::returnKey));
        modalPrimary.onClick = [this] { if (panel == helpPanel) closePanel(); else start(panel == resultPanel ? game.seed : freshSeed()); };
        modalSecondary.onClick = [this] { if (panel == setupPanel) openHelp(); else start(freshSeed()); };
        modalClose.label = ui::utf("×"); modalClose.onClick = [this] { closePanel(); };
        for (auto* button : { &reset, &newWorld, &setupButton, &help, &previewButton, &detailButton, &historyButton })
            button->quiet = true;
        reset.setTooltip("Restart the same world"); newWorld.setTooltip("Generate a new world");
        previewButton.setTooltip("Show the next pulse's values on the board");
        setSize(1160, 880); setPanel(setupPanel);
    }
    ~Board() override { setLookAndFeel(nullptr); }

    void resized() override
    {
        // Fill the window; independent grid axes keep discs circular on wide displays.
        compact = getHeight() < 760 || getWidth() < 1000;
        float outside = 24, separation = 32;
        float available = getWidth() - outside * 2;
        float railWidth = std::clamp(available * .28f, 292.f, 448.f);
        float boardWidth = available - railWidth - separation;
        float boardHeight = std::max(280.f, getHeight() - 184.f);
        float left = outside;
        top = 18;
        stats = { left, top, boardWidth, 76 };
        boardHeading = { left, top + 82, boardWidth, 30 };
        board = { left, top + 118, boardWidth, boardHeight };
        rail = { left + boardWidth + separation, top, railWidth, boardHeight + 146 };
        footer = { left, board.getBottom() + 10, available, 26 };
        float rw = railWidth, rx = rail.getX();
        float quarter = (rw - 12) / 4;
        reset.setBounds(roundToInt(rx), roundToInt(top + 3), roundToInt(quarter), 30);
        newWorld.setBounds(roundToInt(rx + quarter + 4), roundToInt(top + 3), roundToInt(quarter), 30);
        setupButton.setBounds(roundToInt(rx + 2 * (quarter + 4)), roundToInt(top + 3), roundToInt(quarter), 30);
        help.setBounds(roundToInt(rx + 3 * (quarter + 4)), roundToInt(top + 3), roundToInt(quarter), 30);
        end.setBounds(roundToInt(rx), roundToInt(top + 39), roundToInt(rw - 90), 35);
        reverseButton.setBounds(roundToInt(rail.getRight() - 84), roundToInt(top + 39), 84, 35);
        previewButton.setBounds(roundToInt(board.getRight() - 104), roundToInt(boardHeading.getY()), 104, 28);
        float cardHeight = compact ? 61 : 68;
        float cardWidth = (rw - 12) / 3;
        for (int i = 0; i < 6; ++i)
            tools[i]->setBounds(roundToInt(rx + (i % 3) * (cardWidth + 6)),
                                roundToInt(board.getY() + (i / 3) * (cardHeight + 6)), roundToInt(cardWidth), roundToInt(cardHeight));
        toolInfo = { rx, board.getY() + cardHeight * 2 + 18, rw, compact ? 61.f : 70.f };
        forecast = { rx, toolInfo.getBottom() + (compact ? 8.f : 16.f), rw, expanded ? 93.f : 59.f };
        detailButton.setBounds(roundToInt(rail.getRight() - 80), roundToInt(forecast.getY() + 3), 80, 24);
        timeline = { rx, forecast.getBottom() + 8, rw, 46 };
        objective = { rx, timeline.getBottom() + 10, rw, game.worldSize ? 53.f : 32.f };
        outcome = { rx, objective.getBottom() + 14, rw, compact ? 86.f : 96.f };
        status = { rx, outcome.getBottom() + 10, rw, 42 };
        sidebarViewport.setBounds(roundToInt(rx), roundToInt(board.getY()), roundToInt(rw + 8), roundToInt(board.getHeight()));
        sidebarCanvas.setSize(roundToInt(rw), roundToInt(std::max(board.getHeight(), status.getBottom() - board.getY())));
        for (auto& b : tools) b->setBounds(b->getBounds().translated(-roundToInt(rx), -roundToInt(board.getY())));
        detailButton.setBounds(detailButton.getBounds().translated(-roundToInt(rx), -roundToInt(board.getY())));
        historyButton.setBounds(roundToInt(rx), roundToInt(footer.getY()), roundToInt(rw), 26);
        dialog = Rectangle<float>(560, 494).withCentre(getLocalBounds().toFloat().getCentre());
        sizeBox.setBounds(roundToInt(dialog.getX() + 30), roundToInt(dialog.getY() + 142), 282, 40);
        difficultyBox.setBounds(roundToInt(dialog.getX() + 328), roundToInt(dialog.getY() + 142), 202, 40);
        dynamic.setBounds(roundToInt(dialog.getX() + 30), roundToInt(dialog.getY() + 210), 495, 30);
        worldSize.setBounds(roundToInt(dialog.getX() + 30), roundToInt(dialog.getY() + 276), 495, 30);
        modalPrimary.setBounds(roundToInt(dialog.getX() + 30), roundToInt(dialog.getBottom() - 66), 182, 36);
        modalSecondary.setBounds(roundToInt(dialog.getX() + 228), roundToInt(dialog.getBottom() - 66), 116, 36);
        modalClose.setBounds(roundToInt(dialog.getRight() - 48), roundToInt(dialog.getY() + 16), 30, 30);
    }

    void paint(Graphics& g) override
    {
        g.fillAll(ui::paper);
        auto counts = game.count(); auto pulse = game;
        if (!game.phase) pulse.tick();
        paintHeader(g, counts);
        paintGrid(g, pulse);
        paintFooter(g, pulse);
        if (panel != none) paintPanel(g);
    }

private:
    void paintHeader(Graphics& g, const zero::Counts& c)
    {
        ui::ruleLine(g, stats.getX(), top + 80, footer.getWidth());
        ui::ruleLine(g, stats.getX(), board.getY() - 7, footer.getWidth());
        g.setColour(ui::rule);
        g.fillRect(float(roundToInt(rail.getX() - 16)), top, 1.5f, footer.getBottom() - top);
        const std::array<String, 4> labels {{ "TURN", "ZEROS", "LIVING", "ACTIONS" }};
        const std::array<int, 4> nums {{ game.turn, c.zeros, c.living, game.ap }};
        const std::array<String, 4> totals {{ " / " + String(game.p.turns), "", " / " + String(game.size * game.size), " / " + String(game.apMax) }};
        float width = stats.getWidth() / 4;
        for (int i = 0; i < 4; ++i)
        {
            float x = stats.getX() + width * i;
            ui::caption(g, labels[i], { x, top + 10, width, 16 });
            if (i > 0)
            {
                g.setColour(ui::rule);
                g.fillRect(float(roundToInt(x - 18)), top + 12, 1.25f, 50.f);
            }
            float size = compact ? 30 : 34;
            auto number = String(nums[i]).paddedLeft('0', 2);
            auto colour = i == 1 ? ui::coral : i == 2 ? ui::blue : ui::ink;
            ui::text(g, number, { x, top + 30, width, 37 }, size, colour);
            GlyphArrangement glyphs; glyphs.addLineOfText(ui::font(size), number, 0, 0);
            float used = glyphs.getBoundingBox(0, -1, true).getWidth();
            ui::text(g, totals[i], { x + used + 4, top + 41, std::max(0.f, width - used - 4), 24 }, 12);
        }
        ui::text(g, String(game.size) + ui::utf(" × ") + String(game.size), boardHeading, 13, ui::ink);
        const char* profiles[] = { "Containment", "Tactics", "Strategy", "Fronts", "Ecology" };
        String modes = String(profiles[(game.initial - 5) / 2]) + ui::utf(" · ")
                     + String(game.difficulty == 0 ? "Easy" : game.difficulty == 2 ? "Hard" : "Normal");
        if (game.dynamic) modes += ui::utf(" · Unstable");
        if (game.worldSize) modes += ui::utf(" · Zero-world");
        ui::text(g, modes, { boardHeading.getX() + 61, boardHeading.getY(), boardHeading.getWidth() - 175, 30 }, 10);
        ui::caption(g, "OPERATIONS", { rail.getX(), boardHeading.getY(), 150, 30 });
        ui::text(g, String(game.ap) + (game.ap == 1 ? " action left" : " actions left"),
                 { rail.getRight() - 140, boardHeading.getY(), 140, 30 }, 10, ui::muted, Justification::centredRight);
    }
    void paintGrid(Graphics& g, const zero::Game& pulse)
    {
        static const uint32_t warm[] = { 0xffe9b27d, 0xffe4a572, 0xffdf9767, 0xffd8895d, 0xffcd7a53, 0xffc26f4e, 0xffb25b45, 0xffae5843, 0xffa75140 };
        static const uint32_t cool[] = { 0xff81b6b4, 0xff75acb0, 0xff699fa9, 0xff5b92a4, 0xff5a8ba1, 0xff487998, 0xff436e90, 0xff416485, 0xff3e5b7b };
        auto satellites = game.satellites();
        int upcoming = -1;
        for (int w = 0; w < 4; ++w) if (game.p.weather[w] >= game.turn) { upcoming = w; break; }
        for (int i = 0; i < int(game.cells.size()); ++i)
        {
            auto c = game.cells[i]; int v = c.v, future = pulse.cells[i].v;
            auto r = cellRect(i);
            bool projected = preview && !game.phase && v != future && v != zero::Void;
            auto bg = v == zero::Void ? Colour(0xfff5f4ee)
                    : projected && std::abs(future) < std::abs(v) ? Colour(0xfff3e2dd)
                    : v > 0 ? Colour(0xfff7eee4) : v < 0 ? Colour(0xffecf0f2) : Colour(0xffeceee7);
            g.setGradientFill(ColourGradient(bg.brighter(.015f), r.getX(), r.getY(),
                                            bg.darker(.012f), r.getX(), r.getBottom(), false));
            g.fillRoundedRectangle(r, 3);
            if (v == zero::Void)
            {
                g.saveState(); g.reduceClipRegion(r.toNearestInt()); g.setColour(ui::rule);
                for (float q = r.getX() - r.getHeight(); q < r.getRight(); q += 6)
                    g.drawLine(q, r.getBottom(), q + r.getHeight(), r.getY());
                g.restoreState();
            }
            else
            {
                float diameter = std::min(r.getWidth(), r.getHeight()) * (v == 0 ? .68f : (43 + std::abs(v) * 3.65f) / 100);
                auto centre = r.getCentre();
                if (projected && game.size >= 11) centre += Point<float>(-6, -3);
                auto disc = Rectangle<float>(diameter, diameter).withCentre(centre);
                auto discColour = v == 0 ? Colour(0xff303730) : Colour(v > 0 ? warm[std::abs(v) - 1] : cool[std::abs(v) - 1]);
                g.setColour(ui::ink.withAlpha(.045f)); g.fillEllipse(disc.translated(0, 1.5f));
                g.setGradientFill(ColourGradient(discColour.brighter(.055f), disc.getX(), disc.getY(),
                                                discColour.darker(.025f), disc.getRight(), disc.getBottom(), false));
                g.fillEllipse(disc);
                if (v == 0) { g.setColour(Colour(0xff727c69)); g.drawEllipse(disc.expanded(3), 1.5f); }
                if (c.anchor)
                {
                    g.setColour(Colour(0xff66916d)); g.drawEllipse(disc.expanded(5), 1.5f);
                    ui::text(g, String(c.anchor), { r.getRight() - 17, r.getY() + 2, 14, 14 }, 10, ui::green, Justification::centred);
                }
                auto colour = v == 0 || std::abs(v) > (v > 0 ? 6 : 5) ? ui::white : Colour(0xff131b17);
                ui::text(g, value(v), disc, std::min(23.f, std::min(stepX(), stepY()) * .30f), colour, Justification::centred);
            }
            if (upcoming >= 0 && game.p.weather[upcoming] - game.turn <= 2
                && (upcoming == 1 || upcoming == 3) && game.affected(upcoming, i))
            { g.setColour(Colour(0xff867294)); g.drawRect(r, 1.5f); }
            if (std::find(satellites.begin(), satellites.end(), i) != satellites.end())
            { g.setColour(Colour(0xff9f456c)); g.drawRect(r, 1.5f); }
            if (i == selected || i == hovered || game.valid(operation, selected, i))
            { g.setColour(i == selected ? ui::ink : Colour(0xff729979)); g.drawRoundedRectangle(r.reduced(.5f), 3, i == selected ? 2.f : 1.f); }
            if (projected)
            {
                bool dense = game.size >= 11;
                float badgeWidth = std::min(dense ? 28.f : 36.f, r.getWidth() * .5f);
                float h = dense ? 16.f : 18.f;
                Rectangle<float> badge(r.getRight() - badgeWidth - 1, r.getBottom() - h - 1, badgeWidth, h);
                g.setColour(future == 0 ? ui::ink : ui::white); g.fillRoundedRectangle(badge, 2);
                ui::text(g, (dense ? String() : ui::utf("→")) + value(future), badge, dense ? 9 : 10,
                         future == 0 ? ui::white : Colour(0xff773e30), Justification::centred);
            }
        }
        for (auto w : game.walls)
        {
            int a = w.first.first, b = w.first.second;
            g.setColour(Colour(0xff719675));
            if (a / game.size == b / game.size)
            {
                float x = board.getX() + (b % game.size) * stepX() - gap() / 2;
                g.drawLine(x, board.getY() + (a / game.size + .19f) * stepY(), x,
                           board.getY() + (a / game.size + .81f) * stepY(), w.second >= 2 ? 6 : 4);
            }
            else
            {
                float y = board.getY() + (b / game.size) * stepY() - gap() / 2;
                g.drawLine(board.getX() + (a % game.size + .19f) * stepX(), y,
                           board.getX() + (a % game.size + .81f) * stepX(), y, w.second >= 2 ? 6 : 4);
            }
        }
    }
    void paintSidebar(Graphics& g, const zero::Game& result)
    {
        // Keep the selected operation's explanation beside the board, before the forecast.
        g.setColour(ui::accents[operation].withAlpha(.055f)); g.fillRoundedRectangle(toolInfo, 4);
        g.setColour(ui::accents[operation].withAlpha(.6f));
        g.fillRoundedRectangle(toolInfo.getX(), toolInfo.getY() + 9, 2, toolInfo.getHeight() - 18, 1);
        ui::icon(g, operation, { toolInfo.getX() + 12, toolInfo.getY() + 9, 15, 15 }, ui::accents[operation]);
        ui::caption(g, ui::names[operation].toUpperCase() + (game.mutated(operation) ? " / CORRUPTED" : ""),
                    { toolInfo.getX() + 35, toolInfo.getY() + 8, toolInfo.getWidth() - 112, 17 }, ui::accents[operation]);
        ui::text(g, String(operation == 4 ? 2 : 1) + (operation == 4 ? " actions" : " action"),
                 { toolInfo.getRight() - 80, toolInfo.getY() + 8, 68, 17 }, 10, ui::muted, Justification::centredRight);
        ui::paragraph(g, toolDescription(), toolInfo.reduced(12, 0).withY(toolInfo.getY() + 28).withHeight(toolInfo.getHeight() - 30), compact ? 11 : 12, ui::sub, 2);
        ui::ruleLine(g, rail.getX(), forecast.getY(), rail.getWidth());
        ui::caption(g, "UPCOMING", { forecast.getX(), forecast.getY() + 6, 180, 18 });
        String title = "Containment", description = "Keep the same single zero through the final three turns.";
        int next = 100;
        const String wn[] = { "Inversion", "Subtract one", "Divide by two", "Modulo five" };
        const String wd[] = { "The whole board changes sign.", "Three central rows subtract 1.",
                              "Magnitudes halve, rounding away from zero.", "One column takes its signed remainder." };
        for (int i = 0; i < 4; ++i) if (game.p.weather[i] >= game.turn && game.p.weather[i] < next)
        { next = game.p.weather[i]; title = wn[i]; description = wd[i]; }
        for (int i = 0; i < 3; ++i) if (game.p.mutation[i] >= game.turn && game.p.mutation[i] < next)
        { next = game.p.mutation[i]; title = String(std::array<const char*, 3>{{ "Invert", "Transfer", "Sever" }}[i]) + " changes"; description = "An operation becomes corrupted. Its description updates when the rule takes effect."; }
        if (game.dynamic) for (int i = 0; i < 2; ++i) if (game.p.geometry[i] >= game.turn && game.p.geometry[i] < next)
        { next = game.p.geometry[i]; title = (i == 0) == (game.initial < 13) ? "World expands" : "World contracts"; description = "The outer edges of the world move."; }
        for (int t : game.p.flux) if (t >= game.turn && t < next)
        { next = t; title = "Satellite zeros"; description = "New zeros appear in the marked cells."; }
        auto countdown = next == 100 ? "Final phase" : next == game.turn ? "This turn" : "Turn " + String(next) + ui::utf(" · in ") + String(next - game.turn);
        ui::text(g, title, { forecast.getX(), forecast.getY() + 27, 192, 27 }, compact ? 18 : 20, ui::ink);
        auto countdownArea = Rectangle<float>(forecast.getRight() - 119, forecast.getY() + 30, 119, 22);
        g.setColour(Colour(0xffe9e3ef).withAlpha(.65f)); g.fillRoundedRectangle(countdownArea, 3);
        ui::text(g, countdown, countdownArea.reduced(7, 0), 10, Colour(0xff624979), Justification::centred);
        if (expanded) ui::paragraph(g, description, { forecast.getX(), forecast.getY() + 61, forecast.getWidth(), 30 }, 11, ui::sub, 2);
        int columns = (game.p.turns + 1) / 2;
        float cellWidth = (timeline.getWidth() - (columns - 1) * 4) / columns;
        for (int t = 1; t <= game.p.turns; ++t)
        {
            int i = t - 1;
            Rectangle<float> r(timeline.getX() + (i % columns) * (cellWidth + 4), timeline.getY() + (i / columns) * 25, cellWidth, 21);
            bool weather = std::find(game.p.weather.begin(), game.p.weather.end(), t) != game.p.weather.end();
            g.setColour(t < game.turn ? Colour(0xffd4dccb) : t >= game.p.turns - 2 ? Colour(0xfff1e4df)
                                       : weather ? Colour(0xffe9e3ef) : Colour(0xffedeee5));
            g.fillRoundedRectangle(r, 1);
            if (weather)
            {
                g.setColour(Colour(0xff867294)); g.fillRect(r.withHeight(2));
            }
            if (std::find(game.p.flux.begin(), game.p.flux.end(), t) != game.p.flux.end())
            {
                g.setColour(Colour(0xff9f456c)); g.fillRect(r.withY(r.getBottom() - 2).withHeight(2));
            }
            if (t == game.turn) { g.setColour(ui::green); g.fillRoundedRectangle(r, 2); }
            ui::text(g, String(t), r, 10, t == game.turn ? ui::white : ui::sub, Justification::centred);
        }
        ui::caption(g, "ONE ZERO", { objective.getX(), objective.getY(), 110, 18 });
        ui::text(g, "Final turns " + String(game.p.turns - 2) + ui::utf("–") + String(game.p.turns),
                 { objective.getX(), objective.getY() + 18, 180, 16 }, 11, ui::muted);
        for (int i = 0; i < 3; ++i)
        {
            Rectangle<float> dot(objective.getRight() - 80 + i * 28, objective.getY() + 3, 20, 20);
            g.setColour(i < game.streak ? ui::green : Colour(0xffedf0e8)); g.fillEllipse(dot);
            g.setColour(ui::rule); g.drawEllipse(dot, 1.25f);
            ui::text(g, String(i + 1), dot, 9, i < game.streak ? ui::white : ui::muted, Justification::centred);
        }

        if (game.worldSize)
            ui::text(g, "Space: " + String(game.quiet) + ui::utf("/3 calm · ") + String(game.pressure) + "/2 pressure",
                     { objective.getX(), objective.getY() + 36, objective.getWidth(), 17 }, 10, Colour(0xff624979));
        ui::ruleLine(g, outcome.getX(), outcome.getY(), outcome.getWidth());
        ui::caption(g, "AFTER THIS TURN", { outcome.getX(), outcome.getY() + 8, 180, 18 });
        auto now = game.count(), future = result.count();
        float half = (outcome.getWidth() - 12) / 2;
        for (int i = 0; i < 2; ++i)
        {
            auto r = Rectangle<float>(outcome.getX() + i * (half + 12), outcome.getY() + 31, half, compact ? 53.f : 61.f);
            auto accent = i == 0 ? ui::coral : ui::blue;
            g.setColour(ui::white); g.fillRoundedRectangle(r, 4);
            g.setColour(ui::rule); g.drawRoundedRectangle(r, 4, 1.5f);
            g.setColour(accent.withAlpha(.65f));
            g.fillRoundedRectangle(r.getX() + 1, r.getY() + 9, 2, r.getHeight() - 18, 1);
            ui::caption(g, i == 0 ? "ZEROS" : "LIVING", r.reduced(12, 0).withY(r.getY() + 5).withHeight(15), accent);
            auto n = i == 0 ? now.zeros : now.living, f = i == 0 ? future.zeros : future.living;
            auto changeColour = result.phase == 2 || (i == 0 ? f > n : f < n) ? ui::coral : ui::green;
            float fontSize = compact ? 23 : 26;
            float baseline = r.getY() + 23;
            GlyphArrangement before; before.addLineOfText(ui::font(fontSize), String(n), 0, 0);
            float used = before.getBoundingBox(0, -1, true).getWidth();
            ui::text(g, String(n), { r.getX() + 12, baseline, used + 5, 29 }, fontSize, ui::muted);
            ui::text(g, ui::utf("→"), { r.getX() + used + 20, baseline, 22, 29 }, 17, ui::muted);
            ui::text(g, String(f), { r.getX() + used + 48, baseline, r.getWidth() - used - 54, 29 }, fontSize, changeColour);
            if (f != n)
                ui::text(g, (f > n ? "+" : "") + String(f - n),
                         { r.getRight() - 40, r.getY() + 5, 28, 15 }, 9, changeColour, Justification::centredRight);
        }
        String note = message.isNotEmpty() ? message : game.phase ? String(game.reason)
                      : result.phase == 2 ? String(result.reason) : pulseBirthNote(result);
        if (message.isEmpty() && !game.phase && !result.phase && result.size == game.size && !result.births)
        {
            ui::text(g, "Survival", { status.getX(), status.getY(), 66, 18 }, 10, ui::muted);
            ui::text(g, String(future.living) + " living / " + String(game.minLiving()) + " needed",
                     { status.getX() + 66, status.getY(), status.getWidth() - 66, 18 }, 10, ui::sub, Justification::centredRight);
            Rectangle<float> track(status.getX(), status.getY() + 25, status.getWidth(), 5);
            g.setColour(ui::rule); g.fillRoundedRectangle(track, 1);
            g.setColour(ui::blue.withAlpha(.9f));
            g.fillRoundedRectangle(track.withWidth(track.getWidth() * future.living / float(game.size * game.size)), 1);
            float minimum = track.getX() + track.getWidth() * game.minLiving() / float(game.size * game.size);
            g.setColour(ui::ink); g.drawVerticalLine(roundToInt(minimum), track.getY() - 2, track.getBottom() + 2);
        }
        else ui::paragraph(g, note, status, 11, result.phase == 2 && message.isEmpty() ? ui::coral : ui::sub, 3);
    }
    String pulseBirthNote(const zero::Game& result) const
    {
        if (result.size != game.size) return "World " + String(game.size) + ui::utf(" × ") + String(game.size)
                                             + ui::utf(" → ") + String(result.size) + ui::utf(" × ") + String(result.size);
        if (result.births) return String(result.births) + (result.births == 1 ? " new zero predicted." : " new zeros predicted.");
        return "Minimum living population: " + String(game.minLiving()) + ".";
    }
    void paintFooter(Graphics& g, const zero::Game& pulse)
    {
        ui::ruleLine(g, footer.getX(), footer.getY() - 4, footer.getWidth());
        const std::array<String, 4> labels {{ "Positive", "Negative", "Zero", "Anchored" }};
        const std::array<Colour, 4> colours {{ ui::coral, ui::blue, ui::ink, ui::green }};
        float x = footer.getX();
        for (int i = 0; i < 4; ++i)
        {
            g.setColour(colours[i]);
            if (i == 3) g.drawEllipse(x, footer.getY() + 10, 7, 7, 1);
            else g.fillEllipse(x, footer.getY() + 10, 6, 6);
            ui::text(g, labels[i], { x + 11, footer.getY(), i == 3 ? 65.f : 54.f, 26 }, 10);
            x += i == 3 ? 79 : 71;
        }
        if (hovered >= 0)
            ui::text(g, cellName(hovered) + "  " + value(game.cells[hovered].v)
                     + (preview && pulse.cells[hovered].v != game.cells[hovered].v ? ui::utf(" → ") + value(pulse.cells[hovered].v) : ""),
                     { board.getRight() - 180, footer.getY(), 180, 26 }, 10, ui::sub, Justification::centredRight);
        if (historyOpen && panel == none)
        {
            Rectangle<float> r(rail.getX(), footer.getY() - 174, rail.getWidth(), 170);
            g.setColour(ui::white); g.fillRoundedRectangle(r, 3); g.setColour(ui::rule); g.drawRoundedRectangle(r, 3, 1);
            ui::text(g, "WORLD " + String(game.seed), { r.getX() + 12, r.getY() + 8, r.getWidth() - 24, 20 }, 10, ui::muted);
            int y = 0;
            for (const auto& item : game.log)
                ui::text(g, item, { r.getX() + 12, r.getY() + 34 + 21.f * y++, r.getWidth() - 24, 20 }, 11);
        }
    }
    void paintPanel(Graphics& g)
    {
        g.setColour(ui::paper.withAlpha(.92f)); g.fillAll();
        g.setColour(Colour(0xff26372b).withAlpha(.045f)); g.fillRoundedRectangle(dialog.translated(0, 7).expanded(7), 7);
        g.setColour(ui::white); g.fillRoundedRectangle(dialog, 4);
        g.setColour(ui::rule); g.drawRoundedRectangle(dialog, 4, 1);
        auto x = dialog.getX() + 30, y = dialog.getY(), w = dialog.getWidth() - 60;
        ui::text(g, panel == setupPanel ? "EXPERIMENT SETUP" : panel == helpPanel ? "HOW TO PLAY" : "EXPERIMENT COMPLETE",
                 { x, y + 24, w - 40, 20 }, 10);
        ui::text(g, panel == setupPanel ? "Set the world." : panel == helpPanel ? "Contain the zero."
                    : game.phase == 1 ? "One zero remains." : "The system collapsed.",
                 { x, y + 60, w, 45 }, panel == resultPanel ? 30 : 34, ui::ink);
        if (panel == setupPanel)
        {
            ui::text(g, "START SIZE", { x, y + 118, 282, 18 }, 10);
            ui::text(g, "DIFFICULTY", { x + 298, y + 118, 202, 18 }, 10);
            ui::text(g, "The edges move at scheduled turns.", { x + 28, y + 240, w - 28, 20 }, 12);
            ui::text(g, "Containment creates space; infection consumes it.", { x + 28, y + 306, w - 28, 20 }, 12);
            ui::ruleLine(g, x, y + 351, w);
            zero::Game balance(129741, 5 + 2 * (sizeBox.getSelectedId() - 1), difficultyBox.getSelectedId() - 1);
            const std::array<String, 3> labels {{ "TURNS", "ACTIONS / TURN", "MIN. LIVING" }};
            const std::array<int, 3> nums {{ balance.p.turns, balance.actions(), balance.minLiving() }};
            for (int i = 0; i < 3; ++i)
            {
                auto bx = x + w * i / 3;
                ui::text(g, String(nums[i]), { bx, y + 359, w / 3, 28 }, 23, ui::ink);
                ui::text(g, labels[i], { bx, y + 391, w / 3, 18 }, 9);
            }
        }
        else if (panel == helpPanel)
        {
            ui::paragraph(g, "Preserve exactly one zero on the same cell for the final three turns, with enough nonzero cells surviving.",
                          { x, y + 122, w, 44 }, 14, ui::ink, 2);
            ui::paragraph(g, ui::utf("Unanchored zeros infect neighbours simultaneously. Lines and 2 × 2 colonies spread further. Negative values resist overlapping attacks."),
                          { x, y + 174, w, 44 }, 12, ui::sub, 3);
            const char* rules[] = { "Move magnitude within two cells; heal zero.", "Swap neighbouring cells.",
                                    "Reverse a sign.", "Block an adjacent edge.", "Remove a cell permanently. Costs 2 actions.",
                                    "Prevent a zero spreading for 3 pulses (2 on Hard)." };
            for (int i = 0; i < 6; ++i)
            {
                ui::text(g, String(i + 1) + "  " + ui::names[i], { x, y + 230 + i * 24.f, 115, 20 }, 12, ui::accents[i]);
                ui::text(g, rules[i], { x + 119, y + 230 + i * 24.f, w - 119, 20 }, 11);
            }
            ui::text(g, "Enter: end turn     Z: undo     Esc: clear selection", { x, y + 388, w, 24 }, 11);
        }
        else
        {
            ui::paragraph(g, game.reason, { x, y + 124, w, 60 }, 16, ui::sub, 3);
            auto c = game.count();
            ui::text(g, "LIVING", { x, y + 217, 230, 24 }, 10);
            ui::text(g, String(c.living) + " / " + String(game.size * game.size), { x, y + 246, 230, 40 }, 30, ui::blue);
            ui::text(g, "CONTAINMENT", { x + 260, y + 217, 230, 24 }, 10);
            ui::text(g, String(game.streak) + " / 3", { x + 260, y + 246, 230, 40 }, 30, ui::green);
            ui::text(g, "World " + String(game.seed) + ui::utf(" · turn ") + String(game.turn), { x, y + 348, w, 26 }, 12);
        }
    }

public:
    void mouseMove(const MouseEvent& e) override
    {
        int next = panel == none ? hitCell(e.position) : -1;
        if (next != hovered) { hovered = next; repaint(); }
    }
    void mouseExit(const MouseEvent&) override { hovered = -1; repaint(); }
    void mouseDown(const MouseEvent& e) override
    {
        if (panel != none) return;
        if (historyOpen && !rail.contains(e.position)) { historyOpen = false; update(); }
        int i = hitCell(e.position);
        if (i >= 0) { grabKeyboardFocus(); clickCell(i); }
    }
    bool keyPressed(const KeyPress& key) override
    {
        int code = key.getKeyCode();
        if (panel != none)
        {
            if (code == KeyPress::escapeKey) { closePanel(); return true; }
            return false; // Native modal controls handle Enter and Tab.
        }
        if (code >= '1' && code <= '6') chooseTool(code - '1');
        else if (code == KeyPress::returnKey) nextTurn();
        else if (code == 'Z' || code == 'z') reverse();
        else if (code == KeyPress::escapeKey) { selected = -1; message.clear(); update(); }
        else if (key.getTextCharacter() == '?') openHelp();
        else if (code == KeyPress::leftKey || code == KeyPress::rightKey || code == KeyPress::upKey || code == KeyPress::downKey)
        {
            if (hovered < 0) hovered = 0;
            else
            {
                int x = hovered % game.size, y = hovered / game.size;
                x = std::clamp(x + (code == KeyPress::leftKey ? -1 : code == KeyPress::rightKey ? 1 : 0), 0, game.size - 1);
                y = std::clamp(y + (code == KeyPress::upKey ? -1 : code == KeyPress::downKey ? 1 : 0), 0, game.size - 1);
                hovered = x + y * game.size;
            }
            repaint();
        }
        else if (code == KeyPress::spaceKey && hovered >= 0) clickCell(hovered);
        else return false;
        return true;
    }
    void renderSnapshots(const File& folder)
    {
        folder.createDirectory();
        auto save = [&](const String& name)
        {
            auto image = createComponentSnapshot(getLocalBounds(), true, 2.f);
            FileOutputStream stream(folder.getChildFile(name)); PNGImageFormat().writeImageToStream(image, stream);
        };
        save("setup.png"); start(129741); save("board.png");
    }
};

class App : public JUCEApplication
{
    struct Window : DocumentWindow
    {
        Window() : DocumentWindow("Zero Is Infectious", ui::paper, allButtons)
        {
            setUsingNativeTitleBar(true); setContentOwned(new Board, true); setResizable(true, false);
            setResizeLimits(840, 720, 16000, 16000);
            if (auto* display = Desktop::getInstance().getDisplays().getPrimaryDisplay())
                setBounds(display->userArea);
            else centreWithSize(1160, 880);
            setVisible(true);
        }
        void closeButtonPressed() override { JUCEApplication::getInstance()->systemRequestedQuit(); }
    };
    std::unique_ptr<Window> window;
public:
    const String getApplicationName() override { return "Zero Is Infectious"; }
    const String getApplicationVersion() override { return "1.3"; }
    void initialise(const String& args) override
    {
        window = std::make_unique<Window>();
        if (args.startsWith("--snapshots "))
        {
            auto folder = File(args.fromFirstOccurrenceOf("--snapshots ", false, false).unquoted());
            static_cast<Board*>(window->getContentComponent())->renderSnapshots(folder);
            MessageManager::callAsync([this] { quit(); });
        }
    }
    void shutdown() override { window.reset(); }
};
START_JUCE_APPLICATION(App)
