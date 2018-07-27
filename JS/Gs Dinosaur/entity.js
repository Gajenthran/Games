var Animation = { RUN0:0, RUN1:1, RUN2:2, NRUN:3, JUMP0:4, JUMP1:5, NJUMP:6, NANIM:7 };

/*! \brief All the entities on the game. There is only one : the dinosaur */
function Entity() {
	this.pos = createVector(50, height);
	this.vel = createVector(1, 0);
	this.acc = createVector(0, 0);

	this.dWidth = 50; this.dHeight = 50;
	this.sx = this.sy = 0;
	this.sWidth = 42; this.sHeight = 45;

	this.anim = Animation.RUN0;
	this.alive = true;
	this.canJump = 1;

	this.applyForce = function(force) {
		this.acc.add(force);
	} 

	this.update = function() {
		this.vel.add(this.acc);
		this.pos.add(this.vel);
		this.acc.set(0, 0); 

		if(this.pos.y > height) {
			this.pos.y = height;
			this.vel.y = 0;
			this.canJump = 1;
		}
	}

	this.show = function() {
		if(frameCount % 2 == 0 && this.canJump != 0)
			this.anim = (this.anim + 1) % Animation.NRUN;

		this.sx = this.anim * this.sWidth;
		image(_imgDinosaur,
			this.pos.x, this.pos.y - this.dWidth, this.dWidth, this.dHeight, 
			this.sx, this.sy, this.sWidth, this.sHeight); 
	}
}