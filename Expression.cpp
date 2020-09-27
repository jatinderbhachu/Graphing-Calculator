#include "Expression.h"
#include "SFML/Graphics/BlendMode.hpp"
#include "SFML/Graphics/View.hpp"

ExpressionParser* Expression::parser = ExpressionParser::getParser();
int Expression::accuracyMultiplier = 1000;

Expression::Expression(std::string exp, ImVec4 color) : mExpression(exp), color(color) {
    mPoints.setPrimitiveType(sf::PrimitiveType::LineStrip);
}

Expression::~Expression() {  }

void Expression::eval(sf::RenderWindow* mWindow) {
    mPoints.clear();

    sf::View mView = mWindow->getView();

    double x;

    parser->registerVar("x", x);

    double left = mWindow->mapPixelToCoords(sf::Vector2i(0,0), mView).x;
    double right = mWindow->mapPixelToCoords(sf::Vector2i(mWindow->getSize().x,0), mView).x;
    stepAccuracy = (std::abs(right)+std::abs(left))/accuracyMultiplier;
    std::vector<sf::Vertex> lines;

    // if expression fails to compile, dont bother rendering
    if(!parser->compile(mExpression)) return;

    // only calculate the points that are visible from left to right of screen
    for (x = left; x <= right; x += stepAccuracy)
    {
        const double y = parser->eval();
        mPoints.append(sf::Vertex(sf::Vector2f(x, -y), sf::Color(color.x*255, color.y*255, color.z*255)));
    }
}

void Expression::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(mPoints);
}
