/*
 * Segment.h
 *
 *  Created on: May 23, 2015
 *      Author: lingnan
 */

#ifndef SEGMENT_H_
#define SEGMENT_H_

#include <bb/cascades/Container>
#include <bb/cascades/StackLayout>
#include <Defaults.h>

class Segment : public bb::cascades::Container
{
public:
    typedef Segment ThisClass;
    typedef Container BaseClass;
    template <typename BuilderType, typename BuiltType>
    class TBuilder : public BaseClass::TBuilder<BuilderType, BuiltType>
    {
    protected:
        TBuilder(BuiltType* node) : BaseClass::TBuilder<BuilderType, BuiltType>(node)
        {
        }
    public:

        // apply layout properties to make this container a subsection
        BuilderType& subsection()
        {
            return this->builder().topMargin(Defaults::space())
                    .bottomMargin(Defaults::space())
                    .left(Defaults::space()).right(Defaults::space());
        }

        // a section should be the top level item as it applies
        // top and bottom padding
        BuilderType& section()
        {
            return this->builder().top(Defaults::space()).bottom(Defaults::space());
        }

        BuilderType& leftToRight()
        {
            return this->layout(bb::cascades::StackLayout::create()
                .orientation(bb::cascades::LayoutOrientation::LeftToRight));
        }
    };

   /*!
    * @brief A concrete builder class for constructing a @c Container.
    *
    * To retrieve the builder, call @c Container::create().
    *
    * @since BlackBerry 10.0.0
    */
    class Builder : public TBuilder<Builder, Segment>
    {
    public:
        Builder() : TBuilder<Builder, Segment>(new Segment())
        {
        }
    };

   /*!
    * @brief Creates and returns a builder for constructing a @c Container.
    *
    * Using the builder to create a container:
    *
    * @snippet tad/examples/controls/container/src/ContainerExamples.cpp container1
    *
    * @return A builder used for constructing a container.
    *
    * @since BlackBerry 10.0.0
    */
    static Builder create()
    {
        return Builder();
    }
};

#endif /* SEGMENT_H_ */
