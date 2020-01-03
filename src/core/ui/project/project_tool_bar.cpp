#include "project_tool_bar.h"

#include <ui/design_system/design_system.h>

#include <QAction>


namespace Ui
{

ProjectToolBar::ProjectToolBar(QWidget* _parent)
    : AppBar(_parent)
{
    QAction* menuAction = new QAction(this);
    menuAction->setText("\uf35c");
    addAction(menuAction);
    connect(menuAction, &QAction::triggered, this, &ProjectToolBar::menuPressed);

    designSystemChangeEvent(nullptr);
}

void ProjectToolBar::clearViews()
{
    //
    // Оставляем только кнопку меню
    //

    while (actions().size() > 1) {
        auto action = actions().last();
        removeAction(action);
        action->deleteLater();
    }

    update();
}

void ProjectToolBar::addView(const QString& _mimeType, const QString& _icon, bool _isActive)
{
    QAction* viewAction = new QAction(this);
    viewAction->setText(_icon);
    viewAction->setCheckable(true);
    viewAction->setChecked(_isActive);
    addAction(viewAction);
    connect(viewAction, &QAction::triggered, this, [this, _mimeType] {
        emit viewPressed(_mimeType);
    });

    update();
}

void ProjectToolBar::designSystemChangeEvent(DesignSystemChangeEvent* _event)
{
    AppBar::designSystemChangeEvent(_event);

    setBackgroundColor(DesignSystem::color().primary());
    setTextColor(DesignSystem::color().onPrimary());
}

} // namespace Ui