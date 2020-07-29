#pragma once

#include "abstract_importer.h"


namespace BusinessLayer
{

/**
 * @brief Импортер сценария из файлов Trelby
 */
class CORE_LIBRARY_EXPORT TrelbyImporter : public AbstractImporter
{
public:
    TrelbyImporter() = default;

    /**
     * @brief Сформировать xml-сценария во внутреннем формате
     */
    QVector<Screenplay> importScreenplays(const ImportOptions& _options) const override;
};

} // namespace BusinessLayer