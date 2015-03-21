/**
 * FileSystem class which is inherited by Directory and File which is the
 basis for all interactive items within the virtual file system in JShell.
 * 
 * name is the string representation of the filesystem
 * parent is the parent directory of this filesystem
 * 
 * @author c3chownl, c4paprak, c3tsafat, g3dakouz
 *
 */

package driver;

public class FileSystem {

	protected String name;
	protected Directory parent;

	/**
	 * Default constructor used for creating the root.
	 */
	public FileSystem() {
		this.name = "/";
	}

	/**
	 * Constructor used to create named FileSystem objects who have a parent
	 * directory.
	 * 
	 * @param name
	 *            the name of the new object
	 * @param parentDirectory
	 *            the parent directory of the new objects
	 */
	public FileSystem(String name, Directory parentDirectory) {
		this.setName(name);
		this.addToParent(parentDirectory);
	}

	/**
	 * Sets the parent of the file to a new directory
	 * 
	 * @param parentDirectory
	 *            the new parent directory
	 */
	public void addToParent(Directory parentDirectory) {
		this.parent = parentDirectory;
	}

	/**
	 * Removes the relationship between this object and its parent, making it
	 * inaccessible.
	 */
	public void deleteFileSystem() {
		this.parent = null;
	}

	/**
	 * Checks to see if this object has itself as a parent (the root does this)
	 * 
	 * @param fileSystemObject
	 *            the object to check
	 * @return a boolean expressing whether the parent is itself
	 */
	public boolean equals(FileSystem fileSystemObject) {
		return (this.getName() == fileSystemObject.getName())
				&& (this.getParent() == fileSystemObject.getParent());
	}

	/**
	 * Sets the name of the object
	 * 
	 * @param name
	 *            the new name
	 */
	public void setName(String name) {
		this.name = name;
	}

	/**
	 * Gets the name of the object.
	 * 
	 * @return the name of the object
	 */
	public String getName() {
		return this.name;
	}

	/**
	 * Gets the parent of the object.
	 * 
	 * @return the parent of the object
	 */
	public Directory getParent() {
		return this.parent;
	}
}