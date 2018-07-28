/*!
 * \file sketch.js
 * \brief G's Dinosaur
 * \author PANCHALINGAMOORTHY Gajenthran
 * \date 26 July 2018
 */

var _dinosaur;
var _cactus = [];
var _bg = [2];
var _cactusTime = [75, 150];
var _delay;
var _imgBg;
var _font;

function preload() {
  _imgBg = loadImage("files/background.png");
 	_font = loadFont("files/PressStart2P.ttf");
}

function setup() {
	createCanvas(640, 480);
	_dinosaur = new Entity();
	_dinosaur.init();
	_cactus.push(new Obstacle(
		idObstacle.CACTUS01, 
		_dinosaur.pos.x + width, 
		height));
	_cactus[_cactus.length-1].init();
	for(var i = 0; i < idBg.NTILES; i++) {
		_bg[i] = new Background(i);
		_bg[i].init();
	}
	_delay = _cactusTime[0];	textFont(_font);
}

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

function drawBackground() {
	for(var i = 0; i < idBg.NTILES; i++) {
		_bg[i].update(_dinosaur);
		_bg[i].show(_dinosaur);
	}
}

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

function drawDinosaur() {
	var gravity = createVector(0, 0.3);
	_dinosaur.applyForce(gravity);
	_dinosaur.update();
	_dinosaur.show();
}

function gameOver() {
	textSize(30);
	text("GAME OVER", _dinosaur.pos.x + width/2 - 180, 200);
	noLoop();
}

function showScore() {
	textSize(10);
	text("| " + _dinosaur.score, _dinosaur.pos.x + (width - 200), 50);
}

function mousePressed() {
	if(_bg[idBg.RESTART].clicked(_dinosaur))
		remove();
}

function keyPressed() {
	if(key == ' ' || keyCode == UP_ARROW) {
		if(_dinosaur.canJump) {
			var jump = createVector(0, -10.5);
			_dinosaur.applyForce(jump);
			_dinosaur.canJump = 0;
		}
	}
}