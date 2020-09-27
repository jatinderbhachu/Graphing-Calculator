#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include "imgui/imgui-SFML.h"
#include "imgui/imgui.h"
#include "Expression.h"

#include "imgui/imgui_stdlib.h"

/*
 * Creates the window, manages input and GUI 
 */
class Application {
   public:
      Application();
      ~Application();

      void init();
      void run();

      sf::RenderWindow mWindow;
      sf::View mView;

   private:
      void showGraph();
      void showAxis();
      void evaluateGraph();
      void handleInput();

      void insertExpression();

      std::vector<float> getAxisSteps(float, float) const;

      sf::Vector2f mMousePos;
      std::vector<Expression*> mExpressions;
      std::vector<std::map<std::string, double>> expVars;

      sf::Font mFont;
      sf::Text mGridText;

      ImGuiIO io;
      ImGuiWindowFlags imWindowFlags;
      bool mUpdateOnDrag = true;

};

