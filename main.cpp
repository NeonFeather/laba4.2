#include <QApplication>
#include <QStyleFactory>
#include "interface/mainwindow.hpp"
#include "interface/theme/theme.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Настройки для macOS
    app.setApplicationName("Pascal IDE");
    app.setOrganizationName("PascalIDE");
    
    Theme::applyThemeToApplication();
    
    MainWindow window;
    window.show();
    
    return app.exec();
}