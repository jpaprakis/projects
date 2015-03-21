/**
 * FileMovement contains all the commands necessary to move files and 
 * directories as well as copy files and directories to a new path in the 
 * virtual file system.
 * 
 * @author c3chownl, c4paprak, c3tsafat, g3dakouz
 * 
 */

package driver;

public class FileMovement {

	/**
	 * Moves a file system object to a new a file system object in the virtual
	 * file system.
	 * 
	 * @param oldFileSystem
	 *            the old file system to be moved
	 * @param newFileSystem
	 *            the target location for the move
	 * @param newName
	 *            the name of the new Directory you are moving the object to if
	 *            it does not exist
	 */
	public static void mv(FileSystem oldFileSystem, FileSystem newFileSystem,
			String newName) {

		oldFileSystem.deleteFileSystem();
		// if newFileSystem is a File, it must already exist (since if it
		// doesn't, newFileSystem will be a Directory)
		if (newFileSystem instanceof File) {
			if (oldFileSystem instanceof Directory) {
				AllCommands.raiseError("Cannot move a Directory to a File");
				return;
			} else {

				oldFileSystem.addToParent(newFileSystem.getParent());
				newFileSystem.deleteFileSystem();
			}
		} else {
			oldFileSystem.addToParent((Directory) newFileSystem);
		}
		if (!newName.equals("")) {
			System.out.println("setting name");
			oldFileSystem.setName(newName);
		}

	}

	/**
	 * Copies a file system object to a new file system object in the virtual
	 * file system.
	 * 
	 * @param oldFileSystem
	 *            the old file system to be copied
	 * @param newFileSystem
	 *            the target location of the new copy
	 * @param newName
	 *            the name of the new Directory you are moving the object to if
	 *            it does not exist
	 */
	public static void cp(FileSystem oldFileSystem, FileSystem newFileSystem,
			String newName) {
		Directory newDir;
		// Check if you are copying a file
		if (oldFileSystem instanceof File) {
			File oldFile = (File) oldFileSystem;
			// if newFileSystem is a directory,
			// create a new file (with the same contents) inside it
			if (newFileSystem instanceof Directory) {
				// if the directory exists, create the new File object inside it
				if (newName.equals("")) {
					newDir = (Directory) newFileSystem;
				}
				// if the directory doesn't exist, create a new directory with
				// name newName inside newFileSystem parent, and create the file
				// object inside there
				else {
					newDir = new Directory(newName, (Directory) newFileSystem);
				}
				new File(oldFile.getName(), newDir, oldFile.getContents());
			}
			// otherwise newFileSystem is a file, create a new file
			// (with the same contents) inside its parent and delete
			// newFileSystem (since you are overwriting it)
			else {
				newDir = (Directory) newFileSystem.getParent();
				new File(oldFile.getName(), newDir, oldFile.getContents());
				newFileSystem.deleteFileSystem();
			}
		}
		// otherwise you are copying a Directory
		// check to make sure you are not trying to copy it to a File
		else {
			if (newFileSystem instanceof File) {
				AllCommands.raiseError("Cannot move a Directory to a File");
				return;
			}
			// You are copying a Directory to a Directory
			else {
				((Directory) newFileSystem).copyFirstDirectory(
						(Directory) oldFileSystem).cloneDirectory(
						(Directory) oldFileSystem);
				if (!newName.equals("")) {
					((Directory) newFileSystem).fileSystemNameToObject(
							oldFileSystem.getName()).setName(newName);
				}
			}
		}
	}
}

