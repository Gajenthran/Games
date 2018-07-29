/*! \brief elements of the background. */
var idBg = { 
	ROAD:0, RESTART:1, NTILES:2
};

/*! \brief The Background of the game like Road, Cloud... */
function Background(id) {
	this.id = id;
	this.x = 0; this.y = 30;
	this.sx = 0; this.sy = 0;
	this.sWidth = this.sHeight = 0;
	this.dWidth = this.dHeight = 0;

	this.speed = 1; 

	/*! \brief initialize the data (cordinates particularly) of the background. */
	this.init = function() {
		switch(this.id) {
			case idBg.ROAD:
				this.x = 0; this.y = height;
				this.sy = 56;
				this.dHeight = this.sHeight = 14;
				this.dWidth = this.sWidth = 1200;
				break;
			case idBg.RESTART:
				this.y = 250;
				this.sx = 75;
				this.dWidth = this.sWidth = 36;
				this.dHeight = this.sHeight = 32;
				break;
		}
	}

	/*! \brief updating the data of the background. */
	this.update = function(dinosaur) {
		if(this.id == idBg.RESTART)
			this.x = (dinosaur.pos.x - 50) + width/2 - 20;
		else
			this.x += this.speed;
	}

	/*! \brief return true if the user clicks one of the elements of the background. */
	this.clicked = function(dinosaur) {
		if(	mouseX >= this.x - (dinosaur.pos.x - 50) && 
				mouseY <= this.y &&
				mouseX <= (this.x - (dinosaur.pos.x - 50)) + this.dWidth && 
				mouseY >= this.y - this.dHeight)
			return true;
		return false;
	}

	/*! \brief display the elements of the background. */
	this.show = function(dinosaur) {
		if(this.id == idBg.RESTART && dinosaur.alive)
			return false;
		image(_img, 
			this.x, this.y - this.dHeight, this.dWidth, this.dHeight, 
			this.sx, this.sy, this.sWidth, this.sHeight); 
		return true;
	}
}