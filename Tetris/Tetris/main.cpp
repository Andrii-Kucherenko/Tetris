#include <iostream>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <string>
#include <SFML/Audio.hpp>

using namespace sf;

const int FIELD_HEIGHT = 20;
const int FIELD_WIDTH = 10;

int field[FIELD_HEIGHT][FIELD_WIDTH] = { 0 };

int figures[7][4] =
{
	{ 1, 3, 5, 7 },
	{ 2, 4, 5, 7 },
	{ 3, 5, 4, 6 },
	{ 3, 5, 4, 7 },
	{ 2, 3, 5, 7 },
	{ 3, 5, 7, 6 },
	{ 2, 3, 4, 5 }
};

struct Point
{
public:
	int x, y;
} originalPoints[4], reservePoints[4];

bool check()
{
	for (int i = 0; i < 4; i++)
		if (originalPoints[i].x < 0 || originalPoints[i].x >= FIELD_WIDTH || originalPoints[i].y >= FIELD_HEIGHT)
			return 0;
		else if (field[originalPoints[i].y][originalPoints[i].x])
			return 0;

	return 1;
}

int main()
{
	srand(static_cast<unsigned int>(time(0)));
	rand();

	RenderWindow window(VideoMode(236, 422), "Tetris");

	Texture texture_tetramino, texture_background;
	texture_tetramino.loadFromFile("tetramino.png");
	texture_background.loadFromFile("background.png");

	Sprite sprite_tetramino(texture_tetramino), sprite_background(texture_background);

	sprite_tetramino.setTextureRect(IntRect(0, 0, 18, 18));

	int sideShift = 0;
	bool isRotate = 0;
	bool beginGame = true;
	int typeOfTetramino;
	int score = 0;
	int level = 1;
	bool theEnd = false;

	float timeToMoveDown = 0, delayToMove = 0.45f - (float)level / 100 * 5;

	Clock clock;

	Font font;
	if (!font.loadFromFile("Vermin Vibes 1989.ttf"))
		return EXIT_FAILURE;

	Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(25);
	scoreText.setPosition(125.f, 1.f);
	scoreText.setFillColor(Color::White);

	Text levelText;
	levelText.setFont(font);
	levelText.setCharacterSize(25);
	levelText.setPosition(15.f, 1.f);
	levelText.setFillColor(Color::White);

	Music theme, clearLine, levelUP;
	theme.openFromFile("Tetris3.mp3");
	clearLine.openFromFile("clearLine2.mp3");
	levelUP.openFromFile("levelUP.mp3");
	theme.setLoop(true);
	theme.play();

	while (window.isOpen())
	{
		timeToMoveDown += clock.getElapsedTime().asSeconds();
		clock.restart();

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			if (event.type == Event::KeyPressed)
				if (event.key.code == Keyboard::Up)
					isRotate = true;
				else if (event.key.code == Keyboard::Left)
					sideShift = -1;
				else if (event.key.code == Keyboard::Right)
					sideShift = 1;
		}

		if (Keyboard::isKeyPressed(Keyboard::Down))
			delayToMove = 0.05f;

		for (int i = 0; i < 4; ++i)
		{
			reservePoints[i] = originalPoints[i];
			originalPoints[i].x += sideShift;
		}

		if (!check())
			for (int i = 0; i < 4; ++i)
				originalPoints[i] = reservePoints[i];

		if (isRotate)
		{
			Point rotatePoint = originalPoints[1];
			for (int i = 0; i < 4; i++)
			{
				int x = originalPoints[i].y - rotatePoint.y;
				int y = originalPoints[i].x - rotatePoint.x;
				originalPoints[i].x = rotatePoint.x - x;
				originalPoints[i].y = rotatePoint.y + y;
			}
		}

		if (!check())
			for (int i = 0; i < 4; ++i)
				originalPoints[i] = reservePoints[i];

		if (timeToMoveDown > delayToMove)
		{
			for (int i = 0; i < 4; ++i)
			{
				reservePoints[i] = originalPoints[i];
				originalPoints[i].y += 1;
			}

			if (!check())
			{
				for (int i = 0; i < 4; i++)
					field[reservePoints[i].y][reservePoints[i].x] = 1;

				typeOfTetramino = rand() % 7;

				for (int i = 0; i < 4; ++i)
				{
					originalPoints[i].x = figures[typeOfTetramino][i] % 2;
					originalPoints[i].y = figures[typeOfTetramino][i] / 2;
				}
			}

			timeToMoveDown = 0;
		}

		if (beginGame)
		{
			beginGame = false;
			typeOfTetramino = rand() % 7;

			for (int i = 0; i < 4; i++)
			{
				originalPoints[i].x = figures[typeOfTetramino][i] % 2;
				originalPoints[i].y = figures[typeOfTetramino][i] / 2;
			}
		}

		sideShift = 0;
		isRotate = 0;
		delayToMove = 0.45f - (float)level / 100 * 5;

		int lineToCheck = FIELD_HEIGHT - 1;
		for (int i = FIELD_HEIGHT - 1; i > 0; --i, --lineToCheck)
		{
			int countOfFilledCell = 0;

			for (int j = 0; j < FIELD_WIDTH; ++j)
			{
				if (field[i][j])
					++countOfFilledCell;

				field[lineToCheck][j] = field[i][j];
			}

			if (countOfFilledCell == FIELD_WIDTH)
			{
				++lineToCheck;
				++score;
				clearLine.play();

				if (score % 10 == 0)
				{
					++level;
					levelUP.play();
				}
			}
		}

		window.clear(Color::White);
		window.draw(sprite_background);

		for (int i = 0; i < FIELD_HEIGHT; i++)
			for (int j = 0; j < FIELD_WIDTH; j++)
			{
				if (field[i][j] == 0)
					continue;

				sprite_tetramino.setTextureRect(IntRect(0 * 18, 0, 18, 18));
				sprite_tetramino.setPosition(j * 18.f, i * 18.f);
				sprite_tetramino.move(28, 31);
				window.draw(sprite_tetramino);
			}

		for (int i = 0; i < 4; ++i)
		{
			sprite_tetramino.setTextureRect(IntRect(0 * 18, 0, 18, 18));
			sprite_tetramino.setPosition(originalPoints[i].x * 18.f, originalPoints[i].y * 18.f);
			sprite_tetramino.move(28, 31);
			window.draw(sprite_tetramino);
		}

		levelText.setString("Level: " + std::to_string(level));
		window.draw(levelText);

		scoreText.setString("Score: " + std::to_string(score));
		window.draw(scoreText);

		window.display();
	}

	return 0;
}