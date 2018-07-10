public class Ressources {
	public static final int SZ = 30;
  	public static final int ROAD = 0;
  	public static final int WALL = 1;
  	public static final int PIPE = 2;
  	public static final int COIN = 3;
  	public static final int PORTAL = 4;
  	public static final int OPENPORTAL = 5;
  	public static final int TORCH = 6;
  	public static final int GRAVITY = 7;
  	public static final int SPIKE = 8;
  	public static final int HOLE = 9;
  	public static final int SPAWNEN_A = 10;
  	public static final int SPAWNEN_B = 11;
  	public static final int SPAWNEN_C = 12;
  	public static final int CHECKPOINT = 13;
  	public static final int ACT_CHECKPOINT = 14;
  	public static final int CHEST = 15;
  	public static final int OPEN_CHEST = 16;
  	public static final int CRACKED_WALL = 17;
  	public static final int DOOR = 18;
	protected int typecell;
	protected double x;
	protected double y;
	protected double w;
	protected double h;

	public Ressources(int typecell, double x, double y, double w, double h) {
		this.typecell = typecell;
		this.x = x;
		this.y = y;
		this.w = w;
		this.h = h;
	}

	public void setTypecell(int typecell) {
		this.typecell = typecell;
	}

	public int getTypecell() {
		return this.typecell;
	}

	public double getX() {
		return this.x;
	}

	public void setX(double x) {
		this.x = x;
	}

	public double getY() {
		return this.y;
	}

	public void setY(double y) {
		this.y = y;
	}
}