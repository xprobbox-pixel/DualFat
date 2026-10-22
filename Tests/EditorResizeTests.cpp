#include "PluginEditor.h"
#include <iostream>
#include <cstdlib>

static void require(bool condition, const char *message)
{
    if (!condition) { std::cerr << "FAIL: " << message << std::endl; std::exit(1); }
}
static juce::ComboBox *sizeMenu(juce::Component &component)
{
    if (auto *combo = dynamic_cast<juce::ComboBox *>(&component))
        if (combo->getName() == "UI size") return combo;
    for (auto *child : component.getChildren())
        if (auto *found = sizeMenu(*child)) return found;
    return nullptr;
}
int main()
{
    juce::ScopedJuceInitialiser_GUI gui;
    CobaltProcessor processor;
    {
        CobaltEditor editor(processor);
        require(editor.getWidth() == 840 && editor.getHeight() == 741,
                "new instance must open at 75 percent");
        auto *menu = sizeMenu(editor);
        require(menu != nullptr, "size menu must be reachable");
        for (int percent : {50, 150, 60, 100, 75})
        {
            menu->setSelectedId(percent, juce::sendNotificationSync);
            require(editor.getWidth() == juce::roundToInt(1120.0f * percent / 100.0f),
                    "size menu must resize in both directions");
            auto *canvas = editor.getChildComponent(0);
            require(canvas != nullptr, "content canvas exists");
            const auto bounds = editor.getLocalArea(canvas, canvas->getLocalBounds());
            require(editor.getLocalBounds().expanded(1).contains(bounds),
                    "scaled content must fit within the editor");
            auto snapshot = editor.createComponentSnapshot(editor.getLocalBounds());
            require(snapshot.isValid(), "editor must render at every tested size");
        }
        auto bounds = juce::Rectangle<int>(0, 0, 300, 200);
        editor.getConstrainer()->checkBounds(bounds, editor.getBounds(),
                                             {0, 0, 4000, 4000}, false, false, true, true);
        require(bounds.getWidth() >= 560 && bounds.getHeight() >= 494,
                "dragging must respect the smaller minimum");
        menu->setSelectedId(60, juce::sendNotificationSync);
    }
    {
        CobaltEditor reopened(processor);
        require(reopened.getWidth() == 672, "reopening the same instance must retain size");
    }
    std::cout << "Editor tests passed: default, shrink/grow menu, scaled bounds, rendering, "
                 "drag limits and instance recall\n";
}
