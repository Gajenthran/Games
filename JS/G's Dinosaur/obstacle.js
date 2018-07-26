function Obstacle(x, y) {
  this.top = 10;
  this.bottom = 50;
  this.x = x; this.y = y;
  this.w = 80;
  this.speed = 5; 

  this.touch = function(dinosaur) {
    if (dinosaur.pos.y < this.top || dinosaur.pos.y > height - this.bottom)
      if (dinosaur.pos.x > this.x && dinosaur.pos.x < this.x + this.w)
      	dinosaur.alive = false;
  }

  this.update = function() {
    this.x -= this.speed;
  }

  this.offscreen = function() {
    if (this.x < -this.w) {
      return true;
    } else {
      return false;
    }
  }

  this.show = function() {
    fill(255);
    rect(this.x, height-this.bottom, this.w, this.bottom);
  }
}