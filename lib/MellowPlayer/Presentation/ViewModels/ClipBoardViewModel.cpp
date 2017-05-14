#include <QApplication>
#include <QClipboard>
#include "ClipBoardViewModel.hpp"

USE_MELLOWPLAYER_NAMESPACE(Presentation)

ClipBoardViewModel::ClipBoardViewModel(QObject *parent) : QObject(parent) {

}

void ClipBoardViewModel::setText(const QString &text) {
    qApp->clipboard()->setText(text);
}
