import javafx.scene.canvas.GraphicsContext;
import java.util.Random;

public class Enemy extends Entity {
	public static final int RED_ENEMY = 0;
	public static final int BLUE_ENEMY = 1;
	public static final int MOV_SPIKE = 2;
	protected int id; 
	protected boolean death = false;

	public Enemy(double x, double y, int dir, double w, double h, int imgW, int imgH, int id) {
		super(x, y, dir, w, h, imgW, imgH);
		this.id = id;
	}

    public boolean getValueDeath() {
    	return death;
    }

    public void setValueDeath(boolean valueDeath) {
    	this.death = valueDeath;
    }

	public int getId() {
		return id;
	}

	public int getRandomMove() {
		int move;
		Random rand = new Random(System.currentTimeMillis());
		move = rand.nextInt(4);

		return move;
	}

	public int distance (int xPlayer, int yPlayer, int xEnemy, int yEnemy) {
		int x = xPlayer - xEnemy;
		int y = yPlayer - yEnemy;
		return (x * x) + (y * y);
	}

	public int follow(int playerX, int playerY) {
		int best = 4, mDist = Level.H * Ressources.SZ * Level.W * Ressources.SZ, dist;
		int getX = (int)getX()/Ressources.SZ, getY = (int)getY()/Ressources.SZ;

		for (int m = 0; m < NDIR; m++) {
			dist = distance(playerX, playerY, (int)getX() + dirx[m] * Ressources.SZ, (int)getY() + diry[m] * Ressources.SZ);
			
			if (Level.bg.get((getY + diry[m]) * Level.W + (getX + dirx[m])).typecell == Ressources.WALL ||
				Level.bg.get((getY + diry[m]) * Level.W + (getX + dirx[m])).typecell == Ressources.PIPE ||
				m == UP || m == DOWN ||
				(int)getX() + (dirx[m] * Ressources.SZ) < 0 || (int)getX() + dirx[m] * Ressources.SZ > Level.W * Ressources.SZ) {
				dist = Level.W * Ressources.SZ * Level.H * Ressources.SZ;
			}

			if (dist < mDist) {
				mDist = dist;
				best = m;
			}
		}

		return best;
	}

	public int getStandardMove() {
		int m = getDir(), getX = (int)getX() + dirx[m] * Ressources.SZ, getY = (int)getY() + diry[m] * Ressources.SZ;
		int idx = (int)(getY/Ressources.SZ * Level.W + getX/Ressources.SZ), 
			idxG = (int)((getY + diry[3] * Ressources.SZ)/Ressources.SZ * Level.W + getX/Ressources.SZ);
		if(Level.bg.get(idx).typecell == Ressources.WALL || 
			Level.bg.get(idxG).typecell != Ressources.WALL || 
			Level.bg.get(idx).typecell == Ressources.PIPE)
			m = (m == LEFT) ? RIGHT : LEFT;
		
		return m;
	}

	public int getHorizontalMove() {
		int m = getDir(), getX = (int)getX()/Ressources.SZ, getY = (int)getY()/Ressources.SZ;

		if (Level.bg.get((getY + diry[m]) * Level.W + getX).typecell == Ressources.WALL)
			m = (getDir() == UP) ? DOWN : UP;

		return m;
	}

	public void move(Player player) {
		if (getValueDeath() == true)
			return;

		int move, limit = 8, getX = (int)getX()/Ressources.SZ, getY = (int)getY()/Ressources.SZ;
		if(getX() == player.getX() && getY() == player.getY())
			player.lose();

		switch(id) {
			case RED_ENEMY:
				move = follow((int)player.getX(), (int)player.getY());
				break;
			case BLUE_ENEMY:
				move = getStandardMove();
				break;
			case MOV_SPIKE:
				move = getHorizontalMove();
				break;

			default:
				move = getRandomMove();
				if (Level.bg.get((getY + diry[move]) * Level.W + (getX + dirx[move])).typecell == Ressources.WALL ||
					Level.bg.get((getY + diry[move]) * Level.W + (getX + dirx[move])).typecell == Ressources.PIPE ||
					getX + dirx[move] < 0 || getX + dirx[move] > Level.W ||
					getY + diry[move] < 0 || getY + diry[move] > Level.H)
					return;

				break;
		}
		setX((int)getX() + (dirx[move] * Ressources.SZ));
		setY((int)getY() + (diry[move] * Ressources.SZ));
		setDir(move);
		if(getX() == player.getX() && getY() == player.getY())
			player.lose();
	}

	public void respawn(Player player) {
		int distMax = 10 * Ressources.SZ * 10 * Ressources.SZ;
		if (getValueDeath() == true && distance((int)getX(), (int)getY(), (int)player.getX(), (int)player.getY()) > distMax)
			setValueDeath(false);
	}

    public void draw(GraphicsContext gc, int playerX){
    	if (getValueDeath() == true)
    		return;

        int x0 = playerX/Ressources.SZ;
        int srcX = getAnim() * getW(), srcY = 0;

		setAnim((getAnim() + 1) % 3);
        switch(id) {
        	case 0:
        		srcY = (getDir() == 2) ? 0 : getH();
        		break;

        	case 1:
        		srcY = (getDir() == 2) ? getH() * 3 : getH() * 4;
        		break;

        	case 2:
        		srcY = getH() * 2;
        		break;

        }

    	int limitX, moveX = x0 - Level.screenW/2, EnemyX = (int)getX()/Ressources.SZ - moveX;
    	if (moveX < 0)
    		EnemyX = (int)getX()/Ressources.SZ;

    	limitX = Level.screenW + moveX;

    	if (limitX >= Level.W)
    		EnemyX = (int)getX()/Ressources.SZ - moveX + (limitX - Level.W);

        gc.drawImage(getImage(), srcX, srcY, getW(), getH(), EnemyX * Ressources.SZ, y, Ressources.SZ, Ressources.SZ);
    }
}


