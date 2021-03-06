#pragma once

#include "screenplay_text_model_item.h"

#include <Qt>

#include <chrono>

class QDomElement;


namespace BusinessLayer
{

/**
 * @brief Класс элементов папок модели сценария
 */
class CORE_LIBRARY_EXPORT ScreenplayTextModelFolderItem : public ScreenplayTextModelItem
{
public:
    /**
     * @brief Роли данных из модели
     */
    enum DataRole {
        FolderNameRole = Qt::UserRole + 1,
    };

public:
    ScreenplayTextModelFolderItem();
    explicit ScreenplayTextModelFolderItem(const QDomElement& _node);
    ~ScreenplayTextModelFolderItem() override;

    /**
     * @brief Длительность сцены
     */
    std::chrono::milliseconds duration() const;

    /**
     * @brief Определяем интерфейс получения данных сцены
     */
    QVariant data(int _role) const override;

    /**
     * @brief Определяем интерфейс для получения XML блока
     */
    QByteArray toXml() const override;
    QByteArray toXml(ScreenplayTextModelItem* _from, int _fromPosition, ScreenplayTextModelItem* _to, int _toPosition, bool _clearUuid) const;

protected:
    /**
     * @brief Обновляем текст сцены при изменении кого-то из детей
     */
    void handleChange() override;

private:
    class Implementation;
    QScopedPointer<Implementation> d;
};

} // namespace BusinessLayer
