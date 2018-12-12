import javafx.scene.canvas.GraphicsContext;

public class Player extends Entity {
	protected int points;
	protected int life;
	protected boolean gravity;
	protected boolean zone;
	protected boolean win = false;
	protected int sword = 0;
	protected boolean animAttack = false;
	protected boolean checkpoint = false;

	public Player(double x, double y, int dir, double w, double h, int imgW, int imgH, int pts, int life, boolean zone, boolean gravity) {
		super(x, y, dir, w, h, imgW, imgH);
		this.points = pts;
		this.life = life;
		this.zone = zone;
		this.gravity = gravity;
	}

	public int getLife() {
		return life;
	}

	public int getPoints() {
		return points;
	}

	public int getSwordDurability() {
		return sword;
	}
	public boolean getValueWin() {
		return win;
	}

	public void setValueWin (boolean win) {
		this.win = win;
	}

    public void move(int move) {

		double nextx = (double)(getX() + (dirx[move] * Ressources.SZ));
		double nexty = (double)(getY() + (diry[move] *  Ressources.SZ));
    	int idx = (int)(nexty/Ressources.SZ * Level.W + nextx/Ressources.SZ);

		if (nextx < 0.0 || nextx >= (double)(Level.W * Ressources.SZ) || 
			nexty < 0.0 || nexty >= (double)(Level.H * Ressources.SZ) ||
			Level.bg.get(idx).typecell == Ressources.WALL ||
			Level.bg.get(idx).typecell == Ressources.CRACKED_WALL ||
			move == NOTHING && zone == false ||
			(Level.bg.get(idx).typecell != Ressources.PIPE && zone == true))
			return;

		setDir(move);
		setX(nextx);
		if(zone == true)
			setY(nexty);

		isObject((int)getX(), (int)getY());		
    }


	public void gravity() {
		int idx = (int)((getY() + Ressources.SZ)/Ressources.SZ * Level.W + getX()/Ressources.SZ);
		int idxG = (int)((getY() - Ressources.SZ)/Ressources.SZ * Level.W + getX()/Ressources.SZ);
		
		if (getY()/Ressources.SZ < Level.H-1 && 
			zone == false &&
			gravity == false &&
			Level.bg.get(idx).typecell != Ressources.WALL) {
			setY(getY()+Ressources.SZ);
		}

		if(getY()/Ressources.SZ > 0 &&
		   zone == false &&
		   gravity == true &&
		   Level.bg.get(idxG).typecell != Ressources.WALL) {
			setY(getY() - Ressources.SZ);
		}

		isObject(getX(), getY());
	}

	public void isObject(double nextx, double nexty) {
		int idx = (int)(nexty/Ressources.SZ * Level.W + nextx/Ressources.SZ);

		switch(Level.bg.get(idx).typecell) {
			case Ressources.PIPE:
				zone = true;
				break;
			case Ressources.COIN:
				points++;
				Level.bg.get(idx).typecell = Ressources.ROAD;
				break;
			case Ressources.PORTAL:
				transporter(idx, Ressources.OPENPORTAL);
				break;
			case Ressources.GRAVITY:
				gravity = (gravity == true) ? false : true;
				break;
			case Ressources.SPIKE:
				lose();
				break;
			case Ressources.HOLE:
				lose();
				break;
			case Ressources.CHECKPOINT:
				checkpoint = true;
				Level.bg.get(idx).typecell = Ressources.ACT_CHECKPOINT;
				break;
			case Ressources.CHEST:
				openChest(idx);
				break;
			case Ressources.DOOR:
				levelFinished();
				break;
		}
	}

	public void openChest (int idx) {
		if(getDir() == UP) {
			Level.bg.get(idx).typecell = Ressources.OPEN_CHEST;
			sword += 3;
		}
	}
	public void attack(Enemy enemy) {
		if (zone == true || gravity == true || sword == 0)
			return;

		int getX = (int)getX()/Ressources.SZ + dirx[getDir()], getY = (int)getY()/Ressources.SZ + diry[getDir()];
		animAttack = true;

		if((int)getX() + dirx[getDir()] * Ressources.SZ == (int)enemy.getX() &&
		   (int)getY() + diry[getDir()] * Ressources.SZ == (int)enemy.getY() &&
		   enemy.getId() != 2 && enemy.getValueDeath() == false) {
			enemy.setValueDeath(true);
			sword--;
		}

		if(Level.bg.get(getY * Level.W + getX).typecell == Ressources.CRACKED_WALL) {
			Level.bg.get(getY * Level.W + getX).typecell = Ressources.ROAD;
			sword--;
		}

	}

	public void position (double posX, double posY) {
		if (zone == true)
			return;

		int x0 = (int)x/Ressources.SZ;

		int playerX = (int)(Math.ceil(posX/Ressources.SZ)) - 1, playerY = (int)(Math.ceil(posY/Ressources.SZ)) - 1;

		int moveX = x0 - Level.screenW/2;
		if (moveX < 0)
			moveX = 0;

		if (moveX >= Level.W - Level.screenW)
			moveX = Level.W - Level.screenW;
		if(Level.bg.get(playerY * Level.W + playerX + moveX).typecell == Ressources.WALL ||
		   Level.bg.get(playerY * Level.W + playerX + moveX).typecell == Ressources.PIPE)
			return;

		setX((double)(playerX + moveX) * Ressources.SZ);
		setY((double)playerY * Ressources.SZ);
		isObject((double)(playerX + moveX) * Ressources.SZ, (double)playerY * Ressources.SZ);
	}

	public void transporter(int idxTc, int typecell) {
		int nn = Level.bg.size(), nnVal;
		for(int i = 0; i < Level.bg.size(); i++) {
			if(Level.bg.get(i).typecell == typecell && i != idxTc) {

				if((i * i) - (idxTc * idxTc) < (nn * nn))
					nn = i;
			}
		}

		setX(nn%Level.W * Ressources.SZ);
		setY(nn/Level.W * Ressources.SZ);
	}

	public void levelFinished() {
		if(getDir() == UP)
			setValueWin(true);

	}
	public void trampoline() {
		if (gravity == false)
			setY(getY()-(Ressources.SZ * 8));
		else
			setY(getY()+(Ressources.SZ * 8));
	}

	public void quitZone() {
		int idx = (int)(getY()/Ressources.SZ * Level.W + getX()/Ressources.SZ);
		if(zone == true && Level.bg.get(idx).typecell == Ressources.PIPE)
			zone = false;
    }

    public void lose() {
		life--;
		if (checkpoint == true) {
			for(int i = 0; i < Level.bg.size(); i++)
				if(Level.bg.get(i).typecell == Ressources.ACT_CHECKPOINT) {
					setX(i%Level.W * Ressources.SZ);
					setY(i/Level.W * Ressources.SZ);
	    		}

	   		return;
		}
			
		setX(1 * Ressources.SZ);
		setY(3 * Ressources.SZ);
    }

	public void draw(GraphicsContext gc){
		int x0 = (int)x/Ressources.SZ, limitX, moveX = x0 - Level.screenW/2, playerX = x0 - moveX;
		int srcXatt = 23, srcYatt = 144;
		int srcX = getAnim()* getW(), srcY = 0;
		switch(getDir()) {
			case LEFT:
				srcY = (gravity == false) ? getH() : getH() * 8;
				setAnim( (getAnim()+ 1) % 3);
        		if (zone == true) {
					srcY = getH() * 4;
					setAnim( (getAnim()+ 1) % 4);
        		}
        		break;

        	case UP:
        		if(zone == true) {
		        	srcY = getH() * 6;
		        	setAnim( (getAnim()+ 1) % 4);
	        	}
	        	else
	        		srcY = getH() * 2;
	        		srcX = 0;
        		break;

        	case RIGHT:
        		srcY = (gravity == false) ? 0 : getH() * 7;
        		setAnim( (getAnim()+ 1) % 3);
        		if (zone == true) {
        			srcY = getH() * 3;
        			setAnim( (getAnim()+ 1) % 4);
        		}
        		break;

        	case DOWN:
        		if(zone == true) {
		        	srcY = getH() * 5;
		        	setAnim( (getAnim()+ 1) % 4);
	        	}
	        	else {
	        		srcY = getH() * 2;
	        		srcX = getW();
	        	}
	 	    	break;

        	case NOTHING:
        		srcX = (zone == true) ? (gravity == false) ? getW() * 3 : getW() * 4 : (gravity == false) ? getW() : getW() * 2;
        		srcY = getH() * 2;
        		break;
        }
    	if (moveX < 0)
    		playerX = x0;

    	limitX = Level.screenW + moveX;

    	if (limitX >= Level.W)
    		playerX = x0 - moveX + (limitX - Level.W);

    	if (animAttack == true) {
    		if(getDir() == RIGHT)
    	    	gc.drawImage(getImage(), 0, srcYatt, 23, 21, playerX * Ressources.SZ, y, Ressources.SZ * 2, Ressources.SZ);
    		else
    	    	gc.drawImage(getImage(), srcXatt, srcYatt, 23, 21, (playerX + dirx[getDir()]) * Ressources.SZ, y, Ressources.SZ * 2, Ressources.SZ);
		}

    	else
        	gc.drawImage(getImage(), srcX, srcY, getW(), getH(), playerX * Ressources.SZ, y, Ressources.SZ, Ressources.SZ);
        animAttack = false;
    }
}