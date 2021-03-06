#pragma once

#include <QtGui>
#include <QtWidgets>

#include "qitems.h"
#include "qsearch.h"

class QPaletteInner;

struct EnterResult {
    bool hide_;
    QPaletteInner *nextPalette_;

public:
    EnterResult(bool hide) : hide_(hide), nextPalette_(nullptr) {}

    EnterResult(QPaletteInner *nextPalette) : hide_(true), nextPalette_(nextPalette) {}

    bool hide() { return hide_; }

    QPaletteInner *nextPalette() { return nextPalette_; }
};

class QPaletteInner : public QFrame {
protected:
    QItems *entries_;
    QVBoxLayout *layout_;
    QSearch *searchbox_;

    CSSObserver *css_observer_;

public:
    QSearch &searchbox() { return *searchbox_; }

    QItems &entries() { return *entries_; }

    QPaletteInner(QWidget *parent, QObject *);

    void processEnterResult(EnterResult res);

    void onTextChanged(const QString &) {
        entries_->setCurrentIndex(entries_->model()->index(0, 0));
        entries_->scrollToTop();
        entries_->repaint();
    }

    void onEnterPressed() {
        auto res = enter_callback();
        processEnterResult(res);
    }

    bool onArrowPressed(int key) {
        int delta;
        if (key == Qt::Key_Down) {
            delta = 1;
        } else {
            delta = -1;
        }
        auto new_row = entries_->currentIndex().row() + delta;
        if (new_row == -1)
            new_row = 0;
        else if (new_row == entries_->model()->rowCount())
            new_row = 0;
        entries_->setCurrentIndex(entries_->model()->index(new_row, 0));
        return true;
    }

    virtual EnterResult enter_callback() = 0;

    bool eventFilter(QObject *obj, QEvent *event) override {
        switch (event->type()) {
            case QEvent::KeyPress: {
                auto *ke = static_cast<QKeyEvent *>(event);
                switch (ke->key()) {
                    case Qt::Key_Down:
                    case Qt::Key_Up: {
                        event->ignore();
                        return onArrowPressed(ke->key());
                    }
                    case Qt::Key_Enter:
                    case Qt::Key_Return: {
                        event->ignore();
                        onEnterPressed();
                        return true;
                    }
                    default:
                        return QFrame::eventFilter(obj, event);
                }
            }
            case QEvent::ShortcutOverride: {
                event->accept();
                return true;
            }
            default:
                return QFrame::eventFilter(obj, event);
        }
    }

    void keyPressEvent(QKeyEvent *e) override {
        if (e->key() != Qt::Key_Escape)
            QFrame::keyPressEvent(e);
        else {
            if (window())
                window()->close();
        }
    }
};
