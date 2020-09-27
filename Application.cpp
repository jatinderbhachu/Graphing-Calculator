#include "Application.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include <string>


Application::Application() {  }
Application::~Application() { 
}

void Application::init() {

    mFont.loadFromFile("font.ttf");
    mGridText.setFont(mFont);
    mGridText.setCharacterSize(30);
    mGridText.setFillColor(sf::Color::White);

    mWindow.create(sf::VideoMode(1280, 720), "Graphing Calculator", 7, sf::ContextSettings(24, 8, 8, true));
    mView = mWindow.getDefaultView();
    ImGui::SFML::Init(mWindow);

    ImGuiIO& imIO = ImGui::GetIO(); (void)io;
    imIO.WantCaptureMouse = false;


    mWindow.setFramerateLimit(75);
    mWindow.setActive();
    mView.setCenter(0, 0);
    mView.zoom(0.01f);
    mWindow.setView(mView);

    Expression* line = new Expression("sin(x)", ImVec4(1.0, 0, 0, 255));

    mExpressions.push_back(line);

    imWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize ;
    //if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
    //if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
    //if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
    //if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
    //if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
    //if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
    //if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
    //if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
    //if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    //if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

}

void Application::handleInput() {

}

void Application::run() {


    sf::Vector2i pos = sf::Mouse::getPosition();
    sf::Vector2f oldPos;
    bool moving = false;

    sf::Clock deltaClock;

    while (mWindow.isOpen()) {
        sf::Event event;

        while (mWindow.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if(event.type == sf::Event::Closed){
                mWindow.close();
            }

            if(event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    //case sf::Keyboard::Escape:
                        //mWindow.close();
                        //break;
                }
            }

            // dont let application use mouse effects if imgui is using them
            if(!ImGui::GetIO().WantCaptureMouse){
                if(event.type == sf::Event::MouseButtonPressed){
                    // Mouse button is pressed, get the position and set moving as active
                    if (event.mouseButton.button == 0) {
                        //if the draggable point is clicked
                        moving = true;
                        oldPos = mWindow.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                        evaluateGraph();
                    }
                } else if(event.type == sf::Event::MouseButtonReleased){
                    if (event.mouseButton.button == 0) {
                        moving = false;
                        evaluateGraph();
                    }
                } else if(event.type == sf::Event::MouseMoved) {
                    if(moving){
                        const sf::Vector2f newPos = mWindow.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                        const sf::Vector2f deltaPos = oldPos - newPos;
                        mView.setCenter(mView.getCenter() + deltaPos);
                        mWindow.setView(mView);
                        oldPos = mWindow.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                    }
                    pos = sf::Mouse::getPosition(mWindow);  // Get the new position
                    mMousePos = mWindow.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                    if(mUpdateOnDrag) evaluateGraph();
                } else if(event.type == sf::Event::MouseWheelMoved) {
                    if (moving)
                        break;

                    mView.zoom(1.f + event.mouseWheel.delta * -0.1);
                    mWindow.setView(mView);
                    evaluateGraph();
                }
            }
        }

        ImGui::SFML::Update(mWindow, deltaClock.restart());

        ImGui::Begin("Equations", NULL, imWindowFlags);

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::BeginMenu("Options")) {
                    ImGui::Checkbox("Update on drag", &mUpdateOnDrag);
                    ImGui::SliderInt("Graph resolution multiplier", &Expression::accuracyMultiplier, 1000, 15000);
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }


        double left = mWindow.mapPixelToCoords(sf::Vector2i(0,0), mView).x;
        double right = mWindow.mapPixelToCoords(sf::Vector2i(mWindow.getSize().x,0), mView).x;
        float stepAccuracy = (std::abs(right)-std::abs(left))/Expression::accuracyMultiplier;


        if(ImGui::Button("Reset Camera")){
            mView.setCenter(0, 0);

            // 1280 x 720
            mView.setSize(12.8, 7.2);
            mWindow.setView(mView);
            evaluateGraph();
            showAxis();
        }

        ImGui::Text("Mouse Pos: (%1.2f, %1.2f)", mMousePos.x, mMousePos.y);
        //ImGui::Text("Step Accuracy: %1.10f", stepAccuracy);

        //ImGui::Text("Step: %1.2f", stepAccuracy);
        for(int i = 0; i < mExpressions.size(); i++){
            //ImGui::InputText("", mExpressions[i].stringExp, 128);
            ImGui::PushID(i);
            ImGui::InputText("", &mExpressions[i]->mExpression);
            ImGui::SameLine();
            ImGui::ColorEdit4("", (float*)&mExpressions[i]->color, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_DisplayRGB );
            ImGui::SameLine();
            if(ImGui::Button("X")) {
                delete [] mExpressions[i];
                mExpressions.erase(mExpressions.begin() + i);
            }
            ImGui::PopID();

            //ImGui::NewLine();
        }

        if(ImGui::Button("Add +")){
            insertExpression();
        }

        ImGui::End();
        mWindow.clear(sf::Color::Black);

        // BEGIN RENDERING GRAPH

        for(int i = 0; i < mExpressions.size(); i++){
            mWindow.draw(*mExpressions[i]);
        }
        showAxis();

        ImGui::SFML::Render(mWindow);
        mWindow.display();
    }


    ImGui::SFML::Shutdown();

}

void Application::insertExpression() {
    mExpressions.push_back(new Expression("", ImVec4(1.0, 0, 0, 255.0)));
    //evaluateGraph();
}

void Application::evaluateGraph(){
    for(int i = 0; i < mExpressions.size(); i++){
        mExpressions[i]->eval(&mWindow);
    }
}


void Application::showAxis() {
    sf::Vector2f left = mWindow.mapPixelToCoords(sf::Vector2i(0, 0), mView);
    sf::Vector2f right = mWindow.mapPixelToCoords(sf::Vector2i(mWindow.getSize().x, mWindow.getSize().y), mView);
    sf::Vector2f topMiddle = mWindow.mapPixelToCoords(sf::Vector2i(mWindow.getSize().x, 5), mView);
    float top = left.y;
    float bottom = right.y;
    sf::Vertex Y_Axis[] =
        {
            sf::Vertex(sf::Vector2f(0, mWindow.mapPixelToCoords(sf::Vector2i(mWindow.getSize().x, 0), mView).y), sf::Color::White),
            sf::Vertex(sf::Vector2f(0, mWindow.mapPixelToCoords(sf::Vector2i(mWindow.getSize().x, mWindow.getSize().y), mView).y), sf::Color::White)
        };
    sf::Vertex X_Axis[] =
        {
            sf::Vertex(sf::Vector2f(left.x, 0), sf::Color::White),
            sf::Vertex(sf::Vector2f(right.x, 0), sf::Color::White)
        };

    // font scales with window size and while zooming in and out
    float fontScale = (mView.getSize().x/mWindow.getSize().x)/2;
    mGridText.setScale(fontScale, fontScale);
    sf::FloatRect textBounds = mGridText.getLocalBounds();

    std::string coordString = std::to_string(std::ceil(left.x * 100.0f)/100.0f);

    // left middle
    mGridText.setOrigin(0, 0);
    mGridText.setPosition(sf::Vector2f(left.x, 0));
    mGridText.setString(coordString.substr(0, coordString.size() - 4));
    mWindow.draw(mGridText);

    
    // top middle
    mGridText.setOrigin(0, 0);
    coordString = std::to_string(std::ceil(-left.y * 100.0f)/100.0f);
    mGridText.setPosition(sf::Vector2f(0, left.y));
    mGridText.setString(coordString.substr(0, coordString.size() - 4));
    mWindow.draw(mGridText);

    // right middle
    mGridText.setOrigin(textBounds.width, 0);
    coordString = std::to_string(std::ceil(right.x * 100.0f)/100.0f);
    mGridText.setPosition(sf::Vector2f(right.x, 0));
    mGridText.setString(coordString.substr(0, coordString.size() - 4));
    mWindow.draw(mGridText);

    // bottom middle
    mGridText.setOrigin(0, textBounds.height*2);
    coordString = std::to_string(std::ceil(-bottom * 100.0f)/100.0f);
    mGridText.setPosition(sf::Vector2f(0, bottom));

    mGridText.setString(coordString.substr(0, coordString.size() - 4));
    mWindow.draw(mGridText);


    // FIXME: calculate evenly spaced ticks on axis
    /*
    std::vector<float> tickX = getAxisSteps(left.x, right.x);
    for (auto x : tickX) {
        sf::Vertex ticks[]{
            sf::Vertex(sf::Vector2f(x, mWindow.mapPixelToCoords(sf::Vector2i(0, (float)mWindow.mapCoordsToPixel(sf::Vector2f(0, 0), mView).y - 10), mView).y), sf::Color::White),
            sf::Vertex(sf::Vector2f(x, mWindow.mapPixelToCoords(sf::Vector2i(0, (float)mWindow.mapCoordsToPixel(sf::Vector2f(0, 0), mView).y + 10), mView).y), sf::Color::White)};
        //mGridText.setPosition(sf::Vector2f(x, mWindow.mapPixelToCoords(sf::Vector2i(0, (float)mWindow.mapCoordsToPixel(sf::Vector2f(0, 0), mView).y - 20), mView).y));
        std::string xPos = std::to_string(round(x * 1000.00) / 1000.00);
        xPos.erase(xPos.end() - 3, xPos.end());
        //mGridText.setString(xPos);
        //mGridText.setScale(sf::Vector2f(mStep * mAccuracy, mStep * mAccuracy));
        //mWindow.draw(mGridText);

        mWindow.draw(ticks, 2, sf::LineStrip);
    }
    std::vector<float> tickY = getAxisSteps(left.y, right.y);
    for (auto y : tickY) {
        sf::Vertex ticks[]{
            sf::Vertex(sf::Vector2f(mWindow.mapPixelToCoords(sf::Vector2i((float)mWindow.mapCoordsToPixel(sf::Vector2f(0, 0), mView).x - 10, 0), mView).x, y), sf::Color::White),
            sf::Vertex(sf::Vector2f(mWindow.mapPixelToCoords(sf::Vector2i((float)mWindow.mapCoordsToPixel(sf::Vector2f(0, 0), mView).x + 10, 0), mView).x, y), sf::Color::White)};
        //mGridText.setPosition(sf::Vector2f(mWindow.mapPixelToCoords(sf::Vector2i((float)mWindow.mapCoordsToPixel(sf::Vector2f(0, 0), mView).x, -10), mView).x, y));
        std::string yPos = std::to_string(round(-y * 1000.00) / 1000.00);
        yPos.erase(yPos.end() - 3, yPos.end());
        //mGridText.setString(yPos);
        //mGridText.setScale(sf::Vector2f(mStep * mAccuracy, mStep * mAccuracy));
        //mWindow.draw(mGridText);
        mWindow.draw(ticks, 2, sf::LineStrip);
    }
    */




    mWindow.draw(X_Axis, 2, sf::LineStrip);
    mWindow.draw(Y_Axis, 2, sf::LineStrip);
}


// FIXME
std::vector<float> Application::getAxisSteps(float min, float max) const {
    float delta = abs(max) + abs(min);

    std::vector<float> axis;

    float step = delta/10;
    step = pow(10.0, ceil(log10(step)));
    //printf("step: %1.2f", step);


    for(float i = min; i <= max; i+=step){
        axis.push_back((float)i);
    }

    return axis;
}
