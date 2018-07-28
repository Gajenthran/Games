var idObstacle = { 
	CACTUS01:0, CACTUS02:1, CACTUS03:2, NCACTUS:3
};

/*! \brief All the obstacles of the game like Cactus... */
function Obstacle(id, x, y) {
	this.id = id;
	this.pos = createVector(x, y);

	this.sx = id; this.sy = 0;
	this.sWidth = 17; this.sHeight = 35;
	this.dWidth = this.sWidth*2; this.dHeight = this.sHeight*2;

	this.speed = 5; 

	this.init = function() {
		switch(this.id) {
			case idObstacle.CACTUS02:
				this.sx = this.sWidth;
				this.sWidth = 33;
				this.dWidth = this.sWidth*2;
				break;
			case idObstacle.CACTUS03:
				this.sx = 50;
				this.sWidth = 23; this.sHeight = 48;
				this.dHeight = this.sHeight*2; this.dWidth = this.sWidth*2;
				break;
		}
	}

	this.update = function(dinosaur) {
		this.pos.x -= this.speed;
		if(!dinosaur.alive) this.pos.x += this.speed;

		if(	(dinosaur.pos.y > height - this.dHeight) &&
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
		image(_imgBg, 
			this.pos.x, this.pos.y - this.dHeight, this.dWidth, this.dHeight, 
			this.sx, this.sy, this.sWidth, this.sHeight); 
	}
}