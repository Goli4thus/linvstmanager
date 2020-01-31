#include "mainwindow.h"
#include <QApplication>
#include <stdlib.h>
#include "config.h"
#include "runguard.h"
#include <QDebug>

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
        case QtDebugMsg:
#if (D_OUTPUT_DEBUG_MSG == 1)
                fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
#endif
        break;
        case QtInfoMsg:
            fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    }
}

int main(int argc, char *argv[])
{
    RunGuard guard( "some_random_key" );
    if ( !guard.tryToRun() ) {
        qInfo() << "Info: Another instance of LinVstManager is already running.";
        return 0;
    }

    qInstallMessageHandler(customMessageHandler);
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(800, 550);
    w.show();

    return a.exec();
}
