#pragma once
#include "SFML/Graphics/BlendMode.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Shader.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include <SFML/Graphics.hpp>
#include <stdint.h>
#include "imgui.h"
#include "ExpressionParser.h"

/**
 * contains code to calculate points for rendering the equations
 */
class Expression : public sf::Drawable {

    public:
        Expression(std::string exp, ImVec4 color);
        ~Expression();

        static ExpressionParser* parser;
        static int accuracyMultiplier;


        // calculate all points to plot expression
        void eval(sf::RenderWindow* window);

        std::string mExpression;
        ImVec4 color;
    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        sf::VertexArray mPoints;
        float stepAccuracy;
        double x;

};
