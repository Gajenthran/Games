/*!
 * \file sketch.js
 * \brief G's Dinosaur
 * \author PANCHALINGAMOORTHY Gajenthran
 * \date 26 July 2018
 */

/*! \brief the entity controlled by the user. */
var _dinosaur;
/*! \brief the obstacles. the dinosaur has to jump over that. */
var _cactus = [];
/*! \brief the elements of the background. */
var _bg = [];
/*! \brief differents time of respawn (cactus). */
var _cactusTime = [75, 150];
/*! \brief the actual time of the respawn. */
var _delay;
/*! \brief All the images of the game. Entites, background and obstacles */
var _img;
/*! \brief font of the game. */
var _font;

/*! \brief load all the files (image, font). */
function preload() {
 	_img = loadImage("files/background.png");
 	_font = loadFont("files/PressStart2P.ttf");
}

/*! \brief create the canvas. Initialize all the data of the game. */
function setup() {
	createCanvas(640, 480);
	_dinosaur = new Entity();
	_cactus.push(new Obstacle(
		idObstacle.CACTUS01, 
		_dinosaur.pos.x + width, 
		height));
	_cactus[_cactus.length-1].init();
	for(var i = 0; i < idBg.NTILES; i++) {
		_bg.push(new Background(i));
		_bg[i].init();
	}
	_delay = _cactusTime[0];	textFont(_font);
}

/*! \brief display all the elements of the game. */
function draw() {
	background(247);
	translate(-_dinosaur.pos.x + 50, 0);

	drawCactus();
	drawDinosaur();
	drawBackground();
	showScore();

	if(!_dinosaur.alive)
		gameOver();
}

/*! \brief display the background of the game. */
function drawBackground() {
	for(var i = 0; i < idBg.NTILES; i++) {
		_bg[i].update(_dinosaur);
		_bg[i].show(_dinosaur);
	}
}

/*! \brief display the obstacles (cactus) of the game. */
function drawCactus() {
	var delay;
	for(var i = _cactus.length-1; i >= 0; i--) {
		_cactus[i].show();
		_cactus[i].update(_dinosaur);

		if(_cactus[i].offscreen()) {
			_cactus.splice(i, 1);
			_delay = _cactusTime[Math.floor(random(0, _cactusTime.length))];
		}
	}

	if(frameCount % _delay == 0) {
		_cactus.push(new Obstacle(
			Math.floor(random(0, idObstacle.NCACTUS)), 
			_dinosaur.pos.x + width, 
			height));
		_cactus[_cactus.length-1].init();
	}
}

/*! \brief display the entity (dinosaur) of the game. */
function drawDinosaur() {
	var gravity = createVector(0, 0.3);
	_dinosaur.applyForce(gravity);
	_dinosaur.update();
	_dinosaur.show();
}

/*! \brief display the game over screen and stop the loop. */
function gameOver() {
	textSize(30);
	text("GAME OVER", _dinosaur.pos.x + width/2 - 180, 200);
	noLoop();
}

/*! \brief display the score. */
function showScore() {
	textSize(10);
	text("| " + _dinosaur.score, _dinosaur.pos.x + (width - 200), 50);
}

/*! \brief all events when the mouse is pressed. */
function mousePressed() {
	if(_bg[idBg.RESTART].clicked(_dinosaur))
		remove();
}

/*! \brief all events when the key is pressed. */
function keyPressed() {
	if(key == ' ' || keyCode == UP_ARROW) {
		if(_dinosaur.canJump) {
			var jump = createVector(0, -10.5);
			_dinosaur.applyForce(jump);
			_dinosaur.canJump = 0;
		}
	}
}