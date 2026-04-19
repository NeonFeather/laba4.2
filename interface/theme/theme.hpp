#pragma once

#include <QColor>
#include <QPalette>
#include <QFont>
#include <QApplication>

namespace Theme
{

  namespace Colors
  {
    // Background colors
    inline const QColor WindowBackground = QColor(15, 17, 26);    // rgba(22, 22, 22, 255) - #161616
    inline const QColor EditorBackground = QColor(15, 17, 26);    // rgba(15, 17, 26, 255) - #0F111A
    inline const QColor ConsoleBackground = QColor(15, 17, 26);   // rgba(18, 18, 22, 255) - #121216
    inline const QColor SidebarBackground = QColor(10, 12, 21);   // rgba(10, 10, 10, 255) - #0A0A0A
    inline const QColor StatusBarBackground = QColor(10, 12, 21); // rgba(11, 11, 11, 255) - #0B0B0B
    inline const QColor ToolbarBackground = QColor(10, 12, 21);   // rgba(11, 11, 11, 255) - #0B0B0B
    inline const QColor CurrentLineHighlight = QColor(26, 26, 26);
    // Text colors
    inline const QColor TextPrimary = QColor(214, 222, 235);   // rgba(214, 222, 235, 255) - #D6DEEB
    inline const QColor TextSecondary = QColor(140, 157, 184); // rgba(140, 157, 184, 255) - #8C9DB8
    inline const QColor TextDisabled = QColor(75, 90, 110);    // rgba(75, 90, 110, 255) - #4B5A6E

    // Syntax highlighting colors
    inline const QColor Keyword = QColor(199, 146, 234);  // rgba(199, 146, 234, 255) - #C792EA - purple
    inline const QColor Function = QColor(130, 170, 255); // rgba(130, 170, 255, 255) - #82AAFF - blue
    inline const QColor String = QColor(195, 232, 141);   // rgba(195, 232, 141, 255) - #C3E88D - green
    inline const QColor Number = QColor(247, 140, 108);   // rgba(247, 140, 108, 255) - #F78C6C - orange
    inline const QColor Comment = QColor(105, 112, 152);  // rgba(105, 112, 152, 255) - #697098 - gray
    inline const QColor Operator = QColor(137, 221, 255); // rgba(137, 221, 255, 255) - #89DDFF - cyan
    inline const QColor Type = QColor(255, 203, 107);     // rgba(255, 203, 107, 255) - #FFCB6B - yellow
    inline const QColor Variable = QColor(238, 255, 255); // rgba(238, 255, 255, 255) - #EEFFFF - white
    inline const QColor Constant = QColor(247, 140, 108); // rgba(247, 140, 108, 255) - #F78C6C - orange

    // Console output colors
    inline const QColor ConsoleOutput = QColor(214, 222, 235);  // rgba(214, 222, 235, 255) - #D6DEEB
    inline const QColor ConsoleError = QColor(239, 83, 80);     // rgba(239, 83, 80, 255) - #EF5350 - red
    inline const QColor ConsoleWarning = QColor(255, 203, 107); // rgba(255, 203, 107, 255) - #FFCB6B - yellow
    inline const QColor ConsoleSuccess = QColor(195, 232, 141); // rgba(195, 232, 141, 255) - #C3E88D - green
    inline const QColor ConsoleInfo = QColor(130, 170, 255);    // rgba(130, 170, 255, 255) - #82AAFF - blue
    inline const QColor ConsoleDebug = QColor(199, 146, 234);   // rgba(199, 146, 234, 255) - #C792EA - purple

    // UI element colors
    inline const QColor SelectionBackground = QColor(30, 60, 90); // rgba(30, 60, 90, 255) - #1E3C5A
    inline const QColor ScrollbarBackground = QColor(10, 25, 47); // rgba(10, 25, 47, 255) - #0A192F
    inline const QColor ScrollbarHandle = QColor(50, 70, 90);     // rgba(50, 70, 90, 255) - #32465A

    // Border colors
    inline const QColor BorderLight = QColor(22, 23, 32); // rgba(50, 70, 90, 255) - #32465A
    inline const QColor BorderDark = QColor(12, 13, 22);  // rgba(20, 40, 55, 255) - #142837

    // Button colors
    inline const QColor ButtonBackground = QColor(25, 46, 66); // rgba(25, 46, 66, 255) - #192E42
    inline const QColor ButtonHover = QColor(35, 56, 76);      // rgba(35, 56, 76, 255) - #23384C
    inline const QColor ButtonPressed = QColor(45, 66, 86);    // rgba(45, 66, 86, 255) - #2D4256
  }

  // Font settings

  namespace Fonts
  {
    inline const QString EditorFontFamily = "Menlo";
    inline const int EditorFontSize = 13;
    inline const QString ConsoleFontFamily = "Menlo";
    inline const int ConsoleFontSize = 12;
    inline const QString UIFontFamily = "SF Pro Text";
    inline const int UIFontSize = 12;

    inline QFont getEditorFont()
    {
      QFont font(EditorFontFamily, EditorFontSize);
      font.setStyleHint(QFont::Monospace);
      font.setFixedPitch(true);
      return font;
    }

    inline QFont getConsoleFont()
    {
      QFont font(ConsoleFontFamily, ConsoleFontSize);
      font.setStyleHint(QFont::Monospace);
      font.setFixedPitch(true);
      return font;
    }

    inline QFont getUIFont()
    {
      QFont font(UIFontFamily, UIFontSize);
      return font;
    }
  }
  // Style
  namespace StyleSheets
  {

    inline QString getMainWindowStyle()
    {
      return QString(
                 "QMainWindow {"
                 "    background-color: %1;"
                 "}"
                 "QMenuBar {"
                 "    background-color: %2;"
                 "    color: %3;"
                 "    border-bottom: 1px solid %4;"
                 "    padding: 4px;"
                 "}"
                 "QMenuBar::item {"
                 "    background-color: transparent;"
                 "    padding: 4px 8px;"
                 "    border-radius: 4px;"
                 "}"
                 "QMenuBar::item:selected {"
                 "    background-color: %5;"
                 "}"
                 "QMenu {"
                 "    background-color: %2;"
                 "    color: %3;"
                 "    border: 1px solid %4;"
                 "    border-radius: 6px;"
                 "    padding: 4px;"
                 "}"
                 "QMenu::item {"
                 "    padding: 6px 30px 6px 20px;"
                 "    border-radius: 4px;"
                 "}"
                 "QMenu::item:selected {"
                 "    background-color: %5;"
                 "}"
                 "QMenu::separator {"
                 "    height: 1px;"
                 "    background-color: %4;"
                 "    margin: 4px 8px;"
                 "}")
          .arg(
              Colors::WindowBackground.name(),
              Colors::SidebarBackground.name(),
              Colors::TextPrimary.name(),
              Colors::BorderLight.name(),
              Colors::SelectionBackground.name());
    }

    inline QString getToolBarStyle()
    {
      return QString(
                 "QToolBar {"
                 "    background-color: %1;"
                 "    border-bottom: 1px solid %2;"
                 "    spacing: 4px;"
                 "    padding: 4px;"
                 "}"
                 "QToolButton {"
                 "    background-color: transparent;"
                 "    color: %3;"
                 "    border: none;"
                 "    border-radius: 4px;"
                 "    padding: 6px 12px;"
                 "}"
                 "QToolButton:hover {"
                 "    background-color: %4;"
                 "}"
                 "QToolButton:pressed {"
                 "    background-color: %5;"
                 "}")
          .arg(
              Colors::ToolbarBackground.name(),
              Colors::BorderLight.name(),
              Colors::TextPrimary.name(),
              Colors::ButtonHover.name(),
              Colors::ButtonPressed.name());
    }

    inline QString getStatusBarStyle()
    {
      return QString(
                 "QStatusBar {"
                 "    background-color: %1;"
                 "    color: %2;"
                 "    border-top: 1px solid %3;"
                 "}")
          .arg(
              Colors::StatusBarBackground.name(),
              Colors::TextSecondary.name(),
              Colors::BorderLight.name());
    }

    inline QString getEditorStyle()
    {
      return QString(
                 "QPlainTextEdit {"
                 "    background-color: %1;"
                 "    color: %2;"
                 "    border: none;"
                 "    selection-background-color: %3;"
                 "    selection-color: %2;"
                 "}"
                 "QScrollBar:vertical {"
                 "    background-color: %4;"
                 "    width: 12px;"
                 "    border: none;"
                 "}"
                 "QScrollBar::handle:vertical {"
                 "    background-color: %5;"
                 "    border-radius: 6px;"
                 "    min-height: 20px;"
                 "    margin: 2px;"
                 "}"
                 "QScrollBar::handle:vertical:hover {"
                 "    background-color: %6;"
                 "}"
                 "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
                 "    border: none;"
                 "    background: none;"
                 "}")
          .arg(
              Colors::EditorBackground.name(),
              Colors::TextPrimary.name(),
              Colors::SelectionBackground.name(),
              Colors::ScrollbarBackground.name(),
              Colors::ScrollbarHandle.name(),
              Colors::BorderLight.name());
    }

    inline QString getConsoleStyle()
    {
      return QString(
                 "QPlainTextEdit {"
                 "    background-color: %1;"
                 "    color: %2;"
                 "    border: none;"
                 "    selection-background-color: %3;"
                 "    selection-color: %2;"
                 "}")
          .arg(
              Colors::ConsoleBackground.name(),
              Colors::TextPrimary.name(),
              Colors::SelectionBackground.name());
    }

    inline QString getSplitterStyle()
    {
      return QString(
                 "QSplitter::handle {"
                 "    background-color: %1;"
                 "}"
                 "QSplitter::handle:horizontal {"
                 "    width: 2px;"
                 "}"
                 "QSplitter::handle:vertical {"
                 "    height: 2px;"
                 "}")
          .arg(Colors::BorderDark.name());
    }
  }

  // Palette generators

  inline QPalette getApplicationPalette()
  {
    QPalette palette;

    palette.setColor(QPalette::Window, Colors::WindowBackground);
    palette.setColor(QPalette::WindowText, Colors::TextPrimary);
    palette.setColor(QPalette::Base, Colors::EditorBackground);
    palette.setColor(QPalette::AlternateBase, Colors::SidebarBackground);
    palette.setColor(QPalette::ToolTipBase, Colors::SidebarBackground);
    palette.setColor(QPalette::ToolTipText, Colors::TextPrimary);
    palette.setColor(QPalette::Text, Colors::TextPrimary);
    palette.setColor(QPalette::Button, Colors::ButtonBackground);
    palette.setColor(QPalette::ButtonText, Colors::TextPrimary);
    palette.setColor(QPalette::BrightText, Colors::TextPrimary);
    palette.setColor(QPalette::Highlight, Colors::SelectionBackground);
    palette.setColor(QPalette::HighlightedText, Colors::TextPrimary);
    palette.setColor(QPalette::Link, Colors::Function);
    palette.setColor(QPalette::LinkVisited, Colors::Keyword);

    // Disabled colors
    palette.setColor(QPalette::Disabled, QPalette::WindowText, Colors::TextDisabled);
    palette.setColor(QPalette::Disabled, QPalette::Text, Colors::TextDisabled);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Colors::TextDisabled);

    return palette;
  }

  // Theme application functions

  inline void applyThemeToApplication()
  {
    qApp->setPalette(getApplicationPalette());
    qApp->setFont(Fonts::getUIFont());
  }

}