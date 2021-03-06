#include "screenplay_text_model_splitter_item.h"

#include <QDomElement>
#include <QVariant>


namespace BusinessLayer
{

namespace {
    const QString kSplitterTag = QLatin1String("splitter");
    const QString kTypeAttribute = QLatin1String("type");
    const QHash<ScreenplayTextModelSplitterItemType, QString> kSplitterTypeToString
        = {{ ScreenplayTextModelSplitterItemType::Undefined, {} },
           { ScreenplayTextModelSplitterItemType::Start, QStringLiteral("start") },
           { ScreenplayTextModelSplitterItemType::Middle, QStringLiteral("middle") },
           { ScreenplayTextModelSplitterItemType::End, QStringLiteral("end") }};
}

class ScreenplayTextModelSplitterItem::Implementation
{
public:
    Implementation() = default;
    explicit Implementation(ScreenplayTextModelSplitterItemType _type);

    ScreenplayTextModelSplitterItemType type = ScreenplayTextModelSplitterItemType::Undefined;
};

ScreenplayTextModelSplitterItem::Implementation::Implementation(ScreenplayTextModelSplitterItemType _type)
    : type(_type)
{
}


// ****


ScreenplayTextModelSplitterItem::ScreenplayTextModelSplitterItem(ScreenplayTextModelSplitterItemType _type)
    : ScreenplayTextModelItem(ScreenplayTextModelItemType::Splitter),
      d(new Implementation(_type))
{
}

ScreenplayTextModelSplitterItem::ScreenplayTextModelSplitterItem(const QDomElement& _node)
    : ScreenplayTextModelItem(ScreenplayTextModelItemType::Splitter),
      d(new Implementation)
{
    Q_ASSERT(_node.tagName() == kSplitterTag);

    d->type = kSplitterTypeToString.key(_node.attribute(kTypeAttribute));
}

ScreenplayTextModelSplitterItem::~ScreenplayTextModelSplitterItem() = default;

ScreenplayTextModelSplitterItemType ScreenplayTextModelSplitterItem::splitterType() const
{
    return d->type;
}

QByteArray ScreenplayTextModelSplitterItem::toXml() const
{
    if (d->type == ScreenplayTextModelSplitterItemType::Undefined) {
        Q_ASSERT(0);
        return {};
    }

    return QString("<%1 %2=\"%3\"/>\n")
            .arg(kSplitterTag, kTypeAttribute, kSplitterTypeToString.value(d->type)).toUtf8();
}

} // namespace BusinessLayer
