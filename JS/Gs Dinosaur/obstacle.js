/*! \brief All the obstacles of the game like Cactus */
function Obstacle(x, y) {
	this.pos = createVector(x, y);
	this.top = 10;
	this.bottom = 50;
	this.dHeight = 60;
	this.dWidth = 80;
	this.speed = 5; 

	this.update = function(dinosaur) {
		this.pos.x -= this.speed;
		if( (dinosaur.pos.y > height - this.dHeight) &&
				(dinosaur.pos.x + dinosaur.dWidth > this.pos.x && 
				 dinosaur.pos.x < this.pos.x + this.dWidth) )
				dinosaur.alive = false;
	}

	this.offscreen = function() {
		if(this.pos.x < -this.dWidth)
			return true;
		return false;
	}

	this.show = function() {
		fill(255);
		rect(this.pos.x, height-this.bottom, this.dWidth, this.bottom);
	}
}