#include <QtCore>
#ifdef Q_OS_WIN32
    #include <Windows.h>
#endif
#include <MellowPlayer/Application/Logging/LoggingManager.hpp>
#include <MellowPlayer/Infrastructure/Logging/SpdLoggerFactory.hpp>
#include <MellowPlayer/Presentation/Models/QtApplicationModel.hpp>
#include <MellowPlayer/Infrastructure/Applications/SingleInstanceApplication.hpp>
#include "DI.hpp"

namespace di = boost::di;
using namespace std;
USE_MELLOWPLAYER_NAMESPACE(Application)
USE_MELLOWPLAYER_NAMESPACE(Presentation)
USE_MELLOWPLAYER_NAMESPACE(Infrastructure)


int main(int argc, char** argv)
{
    // Init resources embedded in static libraries
    Q_INIT_RESOURCE(use_cases);
    Q_INIT_RESOURCE(presentation);

    SpdLoggerFactory loggerFactory;
    LoggingManager::initialize(loggerFactory, LogLevel::Debug);
    QtApplicationModel qtApp(argc, argv);
    ScopedScope scope{};
    auto injector = di::make_injector(
        di::bind<IQtApplication>().to(qtApp),
        defaultInjector(scope),
        platformInjector(scope),
        notificationPresenterInjector(scope)
    );

#ifdef QT_DEBUG
    IApplication& app = injector.create<IApplication&>();
    app.initialize();
#else
    SingleInstanceApplication& app = injector.create<SingleInstanceApplication&>();
#endif
    return app.run();
}