import javafx.scene.image.Image;

public class Entity {
	public static final int LEFT = 0;
	public static final int UP = 1;
	public static final int RIGHT = 2;
	public static final int DOWN = 3;
	public static final int NOTHING = 4;
	public static final int NDIR = 5;
	public final int dirx[] = {-1, 0, 1, 0, 0};
	public final int diry[] = {0, -1, 0, 1, 0};
	private Image image;
	protected int anim = 0;
	protected double x;
	protected double y;
	protected int dir;
	protected double w;
	protected double h;
	protected int imgW;
	protected int imgH;

	public Entity() {
		x = 0.0;
		y = 0.0;
		w = 0.0;
		h = 0.0;
	}

	public Entity(double x, double y, int dir, double w, double h, int imgW, int imgH) {
		this.x = x;
		this.y = y;
		this.dir = dir;
		this.w = w;
		this.h = h;
		this.imgW = imgW;
		this.imgH = imgH;
	}

    public void setImage(Image image) {
        this.image = image;
        w = this.image.getWidth();
        h = this.image.getHeight();
    }

    public void setImage(String filename) {
        Image img = new Image(filename);
        setImage(img);
    }

    public Image getImage() {
    	return this.image;
    }

    public double getX() {
    	return this.x;
    }

    public double getY() {
    	return this.y;
    }

    public int getDir() {
    	return this.dir;
    }

    public void setY(double y) {
    	this.y = y;
    }

    public void setX(double x) {
    	this.x = x;
    }

    public void setDir(int dir) {
    	this.dir = dir;
    }

    public int getW() {
    	return this.imgW;
    }

    public int getH() {
    	return this.imgH;
    }

    public void setAnim(int anim) {
    	this.anim = anim;
    }

    public int getAnim() {
    	return this.anim;
    }
}