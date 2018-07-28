var idBg = { 
	ROAD:0, RESTART:1, NTILES:2
};

/*! \brief The Background of the game like Road, Cloud... */
function Background(id) {
	this.id = id;
	this.pos = createVector(0, 30);

	this.sx = 0; this.sy = 0;
	this.sWidth = this.sHeight = 0;
	this.dWidth = this.dHeight = 0;

	this.speed = 1; 

	this.init = function() {
		switch(this.id) {
			case idBg.ROAD:
				/* verifier la position de la route */
				this.pos.x = 0; this.pos.y = height;
				this.sy = 55;
				this.dHeight = this.sHeight = 14;
				this.dWidth = this.sWidth = 1200;
				break;
			case idBg.RESTART:
				this.sx = 75;
				this.dWidth = this.sWidth = 36;
				this.dHeight = this.sHeight = 32;
				break;
		}
	}

	this.update = function(dinosaur) {
		this.pos.x -= this.speed;
		if(!dinosaur.alive) this.pos.x += this.speed;
	}

	/* this.offscreen = function() {
		if(this.pos.x < -this.dWidth) 
			return true;
		return false;
	} */

	this.show = function() {
		image(_imgBg, 
			this.pos.x, this.pos.y - this.dHeight, this.dWidth, this.dHeight, 
			this.sx, this.sy, this.sWidth, this.sHeight); 
	}
}