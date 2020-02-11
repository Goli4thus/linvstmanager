/*  LinVstManager is a companion application that allows managing VSTs
 *  in conjunction with various VST bridges created by osxmidi (Mark White)
 *  (see https://github.com/osxmidi/LinVst).
 *
 *  LinVstManager: created by Goli4thus (Markus Paintner) (2020)
 *
 *  This file is part of LinVstManager.
 *
 *  LinVstManager is free software: you can
 *  redistribute it and/or modify it under the terms of the GNU General
 *  Public License as published by the Free Software Foundation, either
 *  version 3 of the License, or (at your option) any later version.
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include <QApplication>
#include <cstdlib>
#include "config.h"
#include "runguard.h"
#include <QDebug>
#include "scanresult.h"
#include "vstbucket.h"

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
    qInstallMessageHandler(customMessageHandler);
    qRegisterMetaType<QVector<ScanResult>>();
    qRegisterMetaType<QVector<VstBridge>>();

    RunGuard guard( "some_random_key" );
    if ( !guard.tryToRun() ) {
        qInfo() << "Info: Another instance of LinVstManager is already running.";
        return 0;
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.resize(800, 550);
    w.show();

    return QApplication::exec();
}
