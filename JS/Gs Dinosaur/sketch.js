/*!
 * \file sketch.js
 * \brief G's Dinosaur
 * \author PANCHALINGAMOORTHY Gajenthran
 * \date 26 July 2018
 */

var _dinosaur;
var _cactus = [];
var _imgDinosaur;
var _imgCactus;
var _cactusTime = [75, 150];
var _delay;

function preload() {
  _imgDinosaur = loadImage("files/dinosaur.png");
  _imgCactus = loadImage("files/cactus.png");
}

function setup() {
	createCanvas(640, 480);
	_dinosaur = new Entity();
	_cactus.push(new Obstacle(idObstacle.SMALL, _dinosaur.pos.x + width, _dinosaur.pos.y));
	_cactus[_cactus.length-1].init();
	_delay = _cactusTime[0];
}

function draw() {
	background(247);
	var gravity = createVector(0, 0.3);
	translate(-_dinosaur.pos.x + 50, 0);

	_dinosaur.applyForce(gravity);

	for(var i = _cactus.length-1; i >= 0; i--) {
		_cactus[i].show();
		_cactus[i].update(_dinosaur);

		if(_cactus[i].offscreen()) {
			_cactus.splice(i, 1);
			_delay = _cactusTime[Math.floor(random(0, _cactusTime.length))];
		}
	}

	_dinosaur.update();
	_dinosaur.show();

	/* To see if the dinosaur touch an obstacle */ 
	print(_dinosaur.alive)

	if(frameCount % _delay == 0) {
		_cactus.push(new Obstacle(Math.floor(random(0, idObstacle.NOBS)), _dinosaur.pos.x + width, _dinosaur.pos.y));
		_cactus[_cactus.length-1].init();
	}

}

function keyPressed() {
	if(key == ' ') {
		if(_dinosaur.canJump) {
			var jump = createVector(0, -10.5);
			_dinosaur.applyForce(jump);
			_dinosaur.canJump = 0;
		}
	}
}