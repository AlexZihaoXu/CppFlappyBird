#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

using namespace std;

int score;
bool gameRunning, gameOvered;
float delta;
sf::RenderWindow *window;

sf::Texture *backgroundTexture;
class Bird {
private:
    sf::Texture *texture;
    float y;
    float vel;
    float currentFrame{};
    sf::Clock* animationClock;
    vector<sf::Texture*> frames;

public:
    Bird() {
        animationClock = new sf::Clock;

        for (const auto& path : {
            "res/textures/bird/1-2.png",
            "res/textures/bird/1-3.png",
            "res/textures/bird/1-2.png",
            "res/textures/bird/1-1.png",
        }) {
            auto frame = new sf::Texture();
            frame->loadFromFile(path);
            frames.push_back(frame);
        }
        texture = frames[0];

        y = 400;
        vel = 0;
    }

    ~Bird() {
        for (const auto& ptr : frames) {
            delete ptr;
        }
        delete animationClock;
    }

    sf::FloatRect getRect() {
        auto size = texture->getSize();
        return {
            50, y,
            (float)size.x, (float)size.y
        };
    }

    void flap() {
        if (not gameRunning or gameOvered) return;
        vel = -420;
    }

    void draw() {
        sf::Sprite birdSprite(*texture);
        birdSprite.setRotation(8*(vel / 400));
        birdSprite.setPosition(50, y);

        window->draw(birdSprite);
    }

    void update() {
        currentFrame += delta * 4;
        if (currentFrame > frames.size()) {
            currentFrame -= frames.size();
        }
        texture = frames[(int)currentFrame];
        if (gameRunning) {
            vel += delta * 1200;
            y += vel * delta;

            if (y < 0 or y + texture->getSize().y > backgroundTexture->getSize().y) {
                gameOvered = true;
            }

            if (y + texture->getSize().y > backgroundTexture->getSize().y) {
                y = (float)backgroundTexture->getSize().y - texture->getSize().y;
                vel = 0;
            }
        }
    }

};
Bird *bird;

sf::Texture* upperPipe;
sf::Texture* lowerPipe;
class Pipe {
private:
    float x;
    float y;
    bool scored;

public:
    Pipe() {
        x = (float)(window->getSize().x + upperPipe->getSize().x);
        y = 100.0f + (float)(rand()%5 - 3) * 50;
        scored = false;
    }

    sf::FloatRect getUpperRect() const {
        auto size = upperPipe->getSize();
        return {
                x, y + 340,
                (float)size.x, (float)size.y
        };
    }

    sf::FloatRect getLowerRect() const {
        auto size = upperPipe->getSize();
        return {
                x, y - 340,
                (float)size.x, (float)size.y
        };
    }

    void draw() const {
        sf::Sprite upperSprite(*upperPipe);
        upperSprite.setPosition(x, y+340);
        sf::Sprite lowerSprite(*lowerPipe);
        lowerSprite.setPosition(x, y-340);

        window->draw(upperSprite);
        window->draw(lowerSprite);
    }

    void update() {
        if (not gameRunning or gameOvered) return;

        x -= 100 * delta;
        auto birdRect = bird->getRect();

        if (birdRect.intersects(getUpperRect()) or birdRect.intersects(getLowerRect())) {
            gameOvered = true;
        }

        if (x + upperPipe->getSize().x < birdRect.left and not scored) {
            scored = true;
            score++;
        }

    }

};
vector<Pipe*> pipes;

sf::Texture *groundTexture;
sf::Clock* pipeGeneratingClock;

sf::Font* font;
void setup() {
    srand((unsigned int) time (nullptr));
    pipeGeneratingClock = new sf::Clock;

    font = new sf::Font();
    font->loadFromFile("res/fonts/04B_19__.TTF");

    window = new sf::RenderWindow(sf::VideoMode(450, 700), "Flappy Bird!");
    window->setPosition({-1157, 275});

    bird = new Bird();

    backgroundTexture = new sf::Texture();
    backgroundTexture->loadFromFile("res/textures/background/day.png");

    groundTexture = new sf::Texture();
    groundTexture->loadFromFile("res/textures/ground.png");

    upperPipe = new sf::Texture();
    lowerPipe = new sf::Texture();

    sf::Image pipeImage;
    pipeImage.loadFromFile("res/textures/pipe.png");
    upperPipe->loadFromImage(pipeImage);
    pipeImage.flipVertically();
    lowerPipe->loadFromImage(pipeImage);

}

void destroy() {
    delete window;
    delete bird;
    delete backgroundTexture;
    delete pipeGeneratingClock;
    delete font;
}

void update() {
    bird->update();
    for (const auto& pipe : pipes) {
        pipe->update();
    }
    if (gameRunning and not gameOvered) {
        if (pipeGeneratingClock->getElapsedTime().asSeconds() > 3.5) {
            pipeGeneratingClock->restart();
            pipes.push_back(new Pipe());

            if (pipes.size() > 4) {
                delete pipes[0];
                pipes.erase(pipes.begin());
            }
        }
    }
}

void handleEvent(sf::Event &event) {
    if (event.type == sf::Event::Closed) {
        window->close();
    }
    if (event.type == sf::Event::MouseButtonPressed) {
        if (not gameRunning) {
            gameRunning = true;
            pipeGeneratingClock->restart();
            pipes.push_back(new Pipe());
        }
        bird->flap();
    }
}

float groundOffset;

void draw() {
    window->clear();
    window->draw(sf::Sprite(*backgroundTexture));

    for (const auto& pipe : pipes) {
        pipe->draw();
    }

    sf::Sprite groundSprite(*groundTexture);
    if (not (not gameRunning or gameOvered)) {
        groundOffset -= delta * 100;
        if (groundOffset <= -24) {
            groundOffset += 24;
        }
    }
    groundSprite.setPosition(groundOffset, backgroundTexture->getSize().y);
    window->draw(groundSprite);

    sf::RectangleShape lowerRectangle({
        (float) window->getSize().x,
        (float) window->getSize().y - backgroundTexture->getSize().y - groundTexture->getSize().y
    });
    lowerRectangle.setPosition(0, (float)backgroundTexture->getSize().y + groundTexture->getSize().y);
    lowerRectangle.setFillColor({245, 228, 138});
    window->draw(lowerRectangle);


    bird->draw();

    sf::Text scoreText("Score: "+to_string(score), *font);
    scoreText.setPosition(window->getSize().x/2-scoreText.getLocalBounds().width/2, 5);
    window->draw(scoreText);

}

int main() {

    setup();

    sf::Clock deltaClock;

    while (window->isOpen()) {
        sf::Event event{};

        while (window->pollEvent(event)) {
            handleEvent(event);
        }

        delta = deltaClock.getElapsedTime().asMicroseconds() / 1000.0f / 1000.0f;
        deltaClock.restart();

        update();
        draw();

        window->display();
    }

    destroy();

    return EXIT_SUCCESS;
}
