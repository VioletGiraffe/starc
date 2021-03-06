#include "screenplay_text_comment_delegate.h"

#include "screenplay_text_comments_model.h"

#include <ui/design_system/design_system.h>

#include <utils/helpers/color_helper.h>
#include <utils/helpers/image_helper.h>
#include <utils/helpers/text_helper.h>

#include <QAbstractItemView>
#include <QDateTime>
#include <QPainter>

using BusinessLayer::ScreenplayTextCommentsModel;


namespace Ui
{

ScreenplayTextCommentDelegate::ScreenplayTextCommentDelegate(QObject* _parent)
    : QStyledItemDelegate(_parent)
{
}

void ScreenplayTextCommentDelegate::setSingleCommentMode(bool _isSingleComment)
{
    m_isSingleCommentMode = _isSingleComment;
}

void ScreenplayTextCommentDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
    //
    // Получим настройки стиля
    //
    QStyleOptionViewItem opt = _option;
    initStyleOption(&opt, _index);

    //
    // Рисуем ручками
    //
    _painter->setRenderHint(QPainter::Antialiasing, true);

    auto backgroundColor = opt.palette.color(QPalette::Base);
    auto textColor = opt.palette.color(QPalette::Text);

    //
    // Рисуем
    //

    //
    // ... фон
    //
    const QRectF backgroundRect = opt.rect;
    if (opt.state.testFlag(QStyle::State_Selected)) {
        //
        // ... для выделенных элементов
        //
        backgroundColor = opt.palette.color(QPalette::Highlight);
        textColor = opt.palette.color(QPalette::HighlightedText);
    } else if (opt.state.testFlag(QStyle::State_MouseOver)) {
        //
        // ... для элементов на которые наведена мышь
        //
        backgroundColor = opt.palette.color(QPalette::AlternateBase);
    } else {
        //
        // ... для остальных элементов
        //
        textColor.setAlphaF(Ui::DesignSystem::inactiveTextOpacity());
    }
    _painter->fillRect(backgroundRect, backgroundColor);

    //
    // ... цвет заметки
    //
    const QRectF colorRect(QPointF(0.0, backgroundRect.top()),
                           QSizeF(Ui::DesignSystem::layout().px4(), backgroundRect.height()));
    _painter->fillRect(colorRect, _index.data(ScreenplayTextCommentsModel::ReviewMarkColorRole).value<QColor>());

    //
    // ... аватар
    //
    const QRectF avatarRect(QPointF(colorRect.right() + Ui::DesignSystem::layout().px16(),
                                    backgroundRect.top() + Ui::DesignSystem::layout().px16()),
                            Ui::DesignSystem::treeOneLineItem().avatarSize());
    const auto avatar = ImageHelper::makeAvatar(_index.data(ScreenplayTextCommentsModel::ReviewMarkAuthorEmailRole).toString(),
                                                Ui::DesignSystem::font().body1(),
                                                avatarRect.size().toSize(),
                                                Qt::white);
    _painter->drawPixmap(avatarRect, avatar, avatar.rect());

    //
    // ... галочка выполнено
    //
    const auto done = _index.data(ScreenplayTextCommentsModel::ReviewMarkIsDoneRole).toBool();
    QRectF iconRect;
    if (m_isSingleCommentMode || done) {
        //
        // ... в режиме единичного комментария также рисуем крестик, который будет закрывать представление с комментарием
        //
        const QSizeF iconSize = Ui::DesignSystem::treeOneLineItem().iconSize();
        iconRect = QRectF(QPointF(backgroundRect.right() - iconSize.width() - Ui::DesignSystem::layout().px12(),
                                  backgroundRect.top() + Ui::DesignSystem::layout().px16() + Ui::DesignSystem::layout().px4()),
                          iconSize);
        _painter->setFont(Ui::DesignSystem::font().iconsMid());
        _painter->setPen(m_isSingleCommentMode ? textColor : Ui::DesignSystem::color().secondary());
        _painter->drawText(iconRect, Qt::AlignCenter, m_isSingleCommentMode ? u8"\U000f0156" : u8"\U000F012C");
        if (m_isSingleCommentMode && done) {
            iconRect.moveRight(iconRect.left());
            _painter->setPen(Ui::DesignSystem::color().secondary());
            _painter->drawText(iconRect, Qt::AlignCenter, u8"\U000F012C");
        }
    }

    //
    // ... пользователь
    //
    _painter->setFont(Ui::DesignSystem::font().subtitle2());
    _painter->setPen(textColor);
    const qreal textLeft = avatarRect.right() + Ui::DesignSystem::layout().px12();
    const qreal textWidth = (iconRect.isEmpty() ? backgroundRect.right() : iconRect.left())
                            - textLeft - Ui::DesignSystem::layout().px12();

    const QRectF textRect(QPointF(textLeft, avatarRect.top()),
                          QSizeF(textWidth, avatarRect.height() / 2));
    const auto text = _painter->fontMetrics().elidedText(
                          _index.data(ScreenplayTextCommentsModel::ReviewMarkAuthorEmailRole).toString(),
                          Qt::ElideRight,
                          static_cast<int>(textRect.width()));
    _painter->drawText(textRect, Qt::AlignLeft | Qt::AlignBottom, text);
    //
    // ... дата
    //
    _painter->setPen(ColorHelper::transparent(textColor, Ui::DesignSystem::disabledTextOpacity()));
    const QRectF dateRect(textRect.bottomLeft(), textRect.size());
    const auto date = _index.data(ScreenplayTextCommentsModel::ReviewMarkCreationDateRole).toDateTime();
    const auto dateText = _painter->fontMetrics().elidedText(date.toString("HH:mm d MMM"), Qt::ElideRight, static_cast<int>(dateRect.width()));
    _painter->drawText(dateRect, Qt::AlignLeft | Qt::AlignTop, dateText);

    //
    // ... комментарий
    //
    const auto comment = _index.data(ScreenplayTextCommentsModel::ReviewMarkCommentRole).toString();
    if (!comment.isEmpty()) {
        const QRectF commentRect(QPointF(avatarRect.left(),
                                         avatarRect.bottom() + Ui::DesignSystem::layout().px12()),
                                 QSizeF(backgroundRect.right() - Ui::DesignSystem::layout().px16() - Ui::DesignSystem::layout().px16() - Ui::DesignSystem::layout().px8(),
                                        backgroundRect.height() - avatarRect.height() - Ui::DesignSystem::layout().px16() * 2 - Ui::DesignSystem::layout().px12()));
        _painter->setFont(Ui::DesignSystem::font().body2());
        _painter->setPen(textColor);
        _painter->drawText(commentRect, Qt::TextWordWrap, comment);
    }
}

QSize ScreenplayTextCommentDelegate::sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
    //
    // Ширина
    //
    int width = _option.rect.width();
    if (const QAbstractItemView* view = qobject_cast<const QAbstractItemView*>(_option.widget)) {
        width = view->viewport()->width();
    }
    width -= Ui::DesignSystem::layout().px8()
             + Ui::DesignSystem::layout().px16()
             + Ui::DesignSystem::layout().px16();

    //
    // Считаем высоту
    //
    // ... высота заголовка: отступ сверху + высота аватара + отступ снизу
    //
    const int headerHeight = Ui::DesignSystem::layout().px16()
                             + Ui::DesignSystem::treeOneLineItem().avatarSize().height()
                             + Ui::DesignSystem::layout().px16();
    //
    // ... высота без комментария
    //
    if ((!m_isSingleCommentMode
         && _index.data(ScreenplayTextCommentsModel::ReviewMarkIsDoneRole).toBool() == true)
        || _index.data(ScreenplayTextCommentsModel::ReviewMarkCommentRole).toString().isEmpty()) {
        return { width, headerHeight };
    }
    //
    // ... полная высота
    //
    const auto comment = _index.data(ScreenplayTextCommentsModel::ReviewMarkCommentRole).toString();
    const int height = headerHeight
                       + Ui::DesignSystem::layout().px12()
                       + TextHelper::heightForWidth(comment, Ui::DesignSystem::font().body2(), width);
    return { width, height };
}

} // namespace Ui
