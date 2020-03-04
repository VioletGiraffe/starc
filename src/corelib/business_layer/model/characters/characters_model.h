#pragma once

#include "../abstract_model.h"


namespace BusinessLayer
{

class CharacterModel;

class CharactersModel : public AbstractModel
{
    Q_OBJECT

public:
    explicit CharactersModel(QObject* _parent = nullptr);
    ~CharactersModel() override;

    /**
     * @brief Добавить модель персонажа
     */
    void addCharacterModel(CharacterModel* _characterModel);

    /**
     * @brief Создать персонажа с заданным именем
     */
    void createCharacter(const QString& _name);

    /**
     * @brief Реализация древовидной модели
     */
    /** @{ */
    QModelIndex index(int _row, int _column, const QModelIndex& _parent = {}) const override;
    QModelIndex parent(const QModelIndex& _child) const override;
    int columnCount( const QModelIndex& _parent = {}) const override;
    int rowCount(const QModelIndex &_parent = {}) const override;
    Qt::ItemFlags flags(const QModelIndex &_index) const override;
    QVariant data(const QModelIndex &_index, int _role) const override;
    /** @} */

protected:
    /**
     * @brief Реализация модели для работы с документами
     */
    /** @{ */
    void initDocument() override;
    void clearDocument() override;
    QByteArray toXml() const override;
    /** @} */

private:
    class Implementation;
    QScopedPointer<Implementation> d;
};

} // namespace BusinessLayer