/*
 * LocaleAwareActionItem.h
 *
 *  Created on: Aug 31, 2015
 *      Author: lingnan
 */

#ifndef LOCALEAWAREACTIONITEM_H_
#define LOCALEAWAREACTIONITEM_H_

#include <bb/cascades/ActionItem>
#include <Utility.h>

class LocaleAwareActionItem : public bb::cascades::ActionItem
{
    Q_OBJECT
public:
    LocaleAwareActionItem(const char *title): _title(title) {
        onTranslatorChanged();
    }
    Q_SLOT void onTranslatorChanged();

    template <typename BuilderType, typename BuiltType>
    class TBuilder : public bb::cascades::ActionItem::TBuilder<BuilderType, BuiltType>
    {
    protected:
          TBuilder(BuiltType* node) : bb::cascades::ActionItem::TBuilder<BuilderType, BuiltType>(node)
          {
          }
    public:
          BuilderType& reloadTitleOn(const QObject* sender, const char *method) {
              conn(sender, method, &this->instance(), SLOT(onTranslatorChanged()));
              return this->builder();
          }
    };
    class Builder : public TBuilder<Builder, LocaleAwareActionItem>
    {
    public:
        explicit Builder(const char *title) :
            TBuilder<Builder, LocaleAwareActionItem>(new LocaleAwareActionItem(title)) {}
    };
    static Builder create(const char *title) {
        return Builder(title);
    }
private:
    const char *_title;
};

#endif /* LOCALEAWAREACTIONITEM_H_ */
