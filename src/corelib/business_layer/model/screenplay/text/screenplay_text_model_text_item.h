#pragma once

#include "screenplay_text_model_item.h"

class QDomElement;


namespace BusinessLayer
{

/**
 * @brief Класс элемента текста модели сценария
 */
class ScreenplayTextModelTextItem : public ScreenplayTextModelItem
{
public:
    ScreenplayTextModelTextItem();
    explicit ScreenplayTextModelTextItem(const QDomElement& _node);
    ~ScreenplayTextModelTextItem() override;

    /**
     * @brief Определяем интерфейс получения данных сцены
     */
    QVariant data(int _role) const override;

    /**
     * @brief Определяем интерфейс для получения XML блока
     */
    QString toXml() const override;

private:
    class Implementation;
    QScopedPointer<Implementation> d;
};

} // namespace BusinessLayer