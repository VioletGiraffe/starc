#include "completer.h"

#include <ui/design_system/design_system.h>
#include <ui/widgets/tree/tree_view.h>
#include <ui/widgets/tree/tree_delegate.h>

#include <QElapsedTimer>
#include <QEvent>
#include <QVariantAnimation>


class Completer::Implementation
{
public:
    explicit Implementation(QWidget* _parent);

    /**
     * @brief Перенастроить внешний вид выпадающего списка
     */
    void reconfigurePopup();


    /**
     * @brief Цвета элеметов списка подстановки
     */
    /** @{ */
    QColor backgroundColor = Qt::red;
    QColor textColor = Qt::red;
    /** @} */

    QElapsedTimer popupTimer;

    /**
     * @brief Виджет со списком автоподстановки
     */
    TreeView* popup = nullptr;

    /**
     * @brief Делегат для отрисовки списка автоподстановки
     */
    TreeDelegate* popupDelegate = nullptr;

    /**
     * @brief Анимация отображения попапа
     */
    QVariantAnimation popupHeightAnimation;
};

Completer::Implementation::Implementation(QWidget* _parent)
    : popup(new TreeView(_parent)),
      popupDelegate(new TreeDelegate(popup))
{
    popup->setHeaderHidden(true);
    popup->setRootIsDecorated(false);
    popup->setMouseTracking(true);
    popup->setFrameShape(QFrame::NoFrame);
    popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    popup->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    popupHeightAnimation.setEasingCurve(QEasingCurve::OutQuint);
    popupHeightAnimation.setDuration(240);
    popupHeightAnimation.setStartValue(0);
    popupHeightAnimation.setEndValue(0);

    connect(&popupHeightAnimation, &QVariantAnimation::valueChanged, popup, [this] (const QVariant& _value) {
        const auto height = _value.toInt();
        popup->resize(popup->width(), height);
    });
    connect(&popupHeightAnimation, &QVariantAnimation::finished, &popupHeightAnimation, [this] {
        popupHeightAnimation.setStartValue(popupHeightAnimation.endValue());
    });
}

void Completer::Implementation::reconfigurePopup()
{
    QPalette palette = popup->palette();
    palette.setColor(QPalette::Base, backgroundColor);
    auto alternateBaseColor = textColor;
    alternateBaseColor.setAlphaF(Ui::DesignSystem::hoverBackgroundOpacity());
    palette.setColor(QPalette::AlternateBase, alternateBaseColor);
    palette.setColor(QPalette::Text, textColor);
    palette.setColor(QPalette::Highlight, Ui::DesignSystem::tree().selectionColor());
    palette.setColor(QPalette::HighlightedText, Ui::DesignSystem::color().secondary());
    popup->setPalette(palette);
    popup->setIndentation(static_cast<int>(Ui::DesignSystem::tree().indicatorWidth()));
}


// ****


Completer::Completer(QWidget* _parent)
    : QCompleter(_parent),
      d(new Implementation(_parent))
{
    d->popup->installEventFilter(this);

    setPopup(d->popup);

    const int maxVisibleItems = 5;
    setMaxVisibleItems(maxVisibleItems);
}

void Completer::setBackgroundColor(const QColor& _color)
{
    d->backgroundColor = _color;
    d->reconfigurePopup();
}

void Completer::setTextColor(const QColor& _color)
{
    d->textColor = _color;
    d->reconfigurePopup();
}

Completer::~Completer() = default;

void Completer::showCompleter(const QRect& _rect)
{
    if (d->popupTimer.elapsed() > 100) {
        d->popupTimer.invalidate();
        d->popupHeightAnimation.setStartValue(0);
        d->popupHeightAnimation.setEndValue(0);
    }

    //
    // Нужно сбросить делегат перед отображением
    //
    d->popup->setItemDelegate(nullptr);
    d->popup->setItemDelegate(d->popupDelegate);

    //
    // Отобразим
    //
    complete(_rect);

    //
    // Анимируем размер попапа
    //
    const int finalHeight = completionCount() * Ui::DesignSystem::treeOneLineItem().height();
    if (d->popupHeightAnimation.state() == QVariantAnimation::Stopped) {
        d->popup->resize(d->popup->width() + Ui::DesignSystem::treeOneLineItem().margins().right(),
                         d->popupHeightAnimation.startValue().toInt());
        d->popupHeightAnimation.setEndValue(finalHeight);
        d->popupHeightAnimation.start();
    } else {
        d->popup->resize(d->popup->width() + Ui::DesignSystem::treeOneLineItem().margins().right(),
                         d->popupHeightAnimation.currentValue().toInt());
        if (d->popupHeightAnimation.endValue().toInt() != finalHeight) {
            d->popupHeightAnimation.stop();
            d->popupHeightAnimation.setStartValue(d->popupHeightAnimation.currentValue());
            d->popupHeightAnimation.setEndValue(finalHeight);
            d->popupHeightAnimation.start();
        }
    }
}

void Completer::closeCompleter()
{
    d->popupTimer.invalidate();
    d->popup->hide();
    d->popupHeightAnimation.setStartValue(0);
    d->popupHeightAnimation.setEndValue(0);
}

bool Completer::eventFilter(QObject* _target, QEvent* _event)
{
    if (_target == d->popup && _event->type() == QEvent::Hide) {
        d->popupTimer.start();
    }

    return QCompleter::eventFilter(_target, _event);
}