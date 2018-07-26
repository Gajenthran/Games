function Entity() {
	this.pos = createVector(50, height);
	this.vel = createVector(1, 0);
	this.acc = createVector(0, 0);
  this.w = this.h = 50;

  this.alive = true;
  this.canJump = 1;

  this.applyForce = function(force) {
  	this.acc.add(force);
  } 

  this.show = function() {
    fill(255);
    rect(this.pos.x, this.pos.y - 50, this.w, this.h);
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
}