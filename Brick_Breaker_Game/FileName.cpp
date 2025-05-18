#include <GL/glut.h>
#include <string>
using namespace std;


const int rows = 6, cols = 13;
const float ballRadius = 0.045f;
const float paddleY = -0.9f;
const float paddleHeight = 0.05f;
const float paddleWidth = 0.4f;
const float paddleSpeed = 0.05f;
float ballX = 0.0f, ballY = paddleY + ballRadius + paddleHeight;
float ballSpeedX = 0.02f, ballSpeedY = 0.02f;
float paddleX = 0.0f, paddleVelocity = 0.0f;
bool bricks[rows][cols], gameOver = false, gameWon = false;
int score = 0;


void drawRectangle(float x, float y, float w, float h, float r, float g, float b) {
	glColor3f(r, g, b);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glEnd();
}

void drawBall() {
	float aspect = 1200.0f / 750.0f;
	glColor3f(0.75f, 0.5f, 1.0f);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(ballX, ballY);
	for (int i = 0; i <= 360; i++) {
		float a = i * 3.14159f / 180.0f;
		glVertex2f(ballX + cos(a) * ballRadius / aspect, ballY + sin(a) * ballRadius);
	}
	glEnd();
}

void drawBricks() {
	float w = 2.0f / cols, h = 0.5f / rows;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (bricks[i][j]) {
				float t = float(i * cols + j) / (rows * cols - 1);
				float r = 0.3f + t * 0.4f;
				float g = 0.2f - t * 0.1f;
				float b = 0.7f + t * 0.3f;
				float bx = 1.0f - (j + 1) * w;
				float by = 1.0f - (i + 1) * h;
				drawRectangle(bx, by, w - 0.01f, h - 0.01f, r, g, b);
			}
		}
	}
}

void drawPaddle() {
	drawRectangle(paddleX - paddleWidth / 2, paddleY, paddleWidth, paddleHeight, 0.2f, 0.6f, 1.0f);
}

void drawText(float x, float y,
	const string& s, float r, float g, float b) {
	glColor3f(r, g, b);
	glRasterPos2f(x, y);
	for (char c : s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}


bool checkWin() {
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			if (bricks[i][j]) return false;
	return true;
}

void initializeBricks() {
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++) bricks[i][j] = true;
	score = 0;
}

void update(int v) {
	if (gameOver || gameWon) return;
	ballX += ballSpeedX;
	ballY += ballSpeedY;
	if (ballX + ballRadius > 1 || ballX - ballRadius < -1) ballSpeedX = -ballSpeedX;
	if (ballY + ballRadius > 1) ballSpeedY = -ballSpeedY;
	if (ballY - ballRadius < -1) gameOver = true;
	if (ballY - ballRadius < paddleY + paddleHeight && ballX > paddleX - paddleWidth / 2 && ballX < paddleX + paddleWidth / 2 && ballSpeedY < 0) {
		float hitPos = (ballX - paddleX) / (paddleWidth / 2);
		ballSpeedX = hitPos * 0.03f;
		ballSpeedY = fabs(ballSpeedY);
	}
	float brickW = 2.0f / cols, brickH = 0.5f / rows;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (!bricks[i][j]) continue;
			float bx = 1.0f - (j + 1) * brickW;
			float by = 1.0f - (i + 1) * brickH;
			if (ballX + ballRadius > bx && ballX - ballRadius < bx + brickW && ballY + ballRadius > by && ballY - ballRadius < by + brickH) {
				bricks[i][j] = false;
				score += 10;
				float overlapLeft = (ballX + ballRadius) - bx;
				float overlapRight = (bx + brickW) - (ballX - ballRadius);
				float overlapTop = (by + brickH) - (ballY - ballRadius);
				float overlapBottom = (ballY + ballRadius) - by;
				bool ballFromLeftRight = (overlapLeft < overlapTop && overlapLeft < overlapBottom) || (overlapRight < overlapTop && overlapRight < overlapBottom);
				if (ballFromLeftRight) ballSpeedX = -ballSpeedX;
				else ballSpeedY = -ballSpeedY;
				break;
			}
		}
	}
	if (checkWin()) gameWon = true;
	paddleX += paddleVelocity;
	if (paddleX - paddleWidth / 2 < -1) paddleX = -1 + paddleWidth / 2;
	if (paddleX + paddleWidth / 2 > 1) paddleX = 1 - paddleWidth / 2;
	glutPostRedisplay();
	glutTimerFunc(16, update, 0);
}


void render() {
	glClear(GL_COLOR_BUFFER_BIT);
	drawBricks();
	drawBall();
	drawPaddle();
	drawText(-0.98f, 0.41f, "Score: " + to_string(score), 1.0f, 1.0f, 0.0f);
	if (gameOver) {
		string msg = "Game Over! Press R to Restart";
		float charWidth = 9.0f / 1280.0f * 2.0f;
		float textWidth = msg.length() * charWidth;
		float x = -textWidth / 2.0f;
		drawText(x, 0.0f, msg, 1.0f, 0.5f, 0.7f);
	}
	else if (gameWon) {
		string msg = "You Won! Press R to Restart";
		float charWidth = 9.0f / 1280.0f * 2.0f;
		float textWidth = msg.length() * charWidth;
		float x = -textWidth / 2.0f;
		drawText(x, 0.0f, msg, 0.7f, 0.9f, 1.0f);
	}
	glutSwapBuffers();
}


void keyboard(int key, int x, int y) {
	if (gameOver || gameWon) return;
	if (key == GLUT_KEY_LEFT) paddleVelocity = -paddleSpeed;
	if (key == GLUT_KEY_RIGHT) paddleVelocity = paddleSpeed;
}
void keyboardUp(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT || key == GLUT_KEY_RIGHT) paddleVelocity = 0;
}

void keyboardNormal(unsigned char key, int x, int y) {
	if (key == 'r' || key == 'R') {
		gameOver = gameWon = false;
		ballX = 0;
		ballY = paddleY + ballRadius + paddleHeight;
		ballSpeedX = 0.02f;
		ballSpeedY = 0.02f;
		initializeBricks();
		glutTimerFunc(16, update, 0);
	}
}


void initOpenGL() {
	glClearColor(0.07f, 0.05f, 0.2f, 1.0f);
	glEnable(GL_LINE_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1, 1, -1, 1);
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1280, 750);
	glutCreateWindow("Brick Breaker Game");
	initOpenGL();
	initializeBricks();
	glutDisplayFunc(render);
	glutTimerFunc(16, update, 0);
	glutSpecialFunc(keyboard);
	glutSpecialUpFunc(keyboardUp);
	glutKeyboardFunc(keyboardNormal);
	glutMainLoop();
	return 0;
}