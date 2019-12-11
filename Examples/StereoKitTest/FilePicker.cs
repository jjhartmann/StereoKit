﻿using StereoKit;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StereoKitTest
{
    class FilePicker
    {
        #region Support types
        public enum FileType
        {
            File,
            Folder
        }
        public struct File
        {
            public FileType type;
            public string   name;
            public string   extension;
        }
        #endregion

        #region Fields
        (string,string)[] filters;
        List<File>        activeFiles = new List<File>();
        int               selectedIndex = -1;
        string            path;
        string            title;
        Pose              windowPose = new Pose(new Vec3(-10,0,0)*Units.cm2m, Quat.LookDir(-Vec3.Forward));
        #endregion

        public string SelectedFile { 
            get => selectedIndex == -1 ? 
                "" : 
                Path.Combine(path, activeFiles[selectedIndex].name + activeFiles[selectedIndex].extension);
        }

        public FilePicker((string name, string filter)[] filters, string initialFolder=null)
        {
            foreach (var s in filters)
                this.title += s+"/";
            this.filters = filters;
            UpdateFolder(initialFolder == null ? Directory.GetCurrentDirectory() : initialFolder);
        }

        void DirectoryUp()
        {
            DirectoryInfo parent = Directory.GetParent(path);
            if (parent != null)
                UpdateFolder(parent.FullName);
        }

        bool Select(int index)
        {
            selectedIndex = index;
            if (activeFiles[index].type == FileType.Folder)
            {
                string newPath = Path.Combine(path, activeFiles[index].name);
                UpdateFolder(newPath);
            } 
            else
            {
                return true;
            }
            return false;
        }

        void UpdateFolder(string newPath)
        {
            path = newPath;

            // Find the files and folders at the current path!
            List<string> files = new List<string>( filters
                .SelectMany(f => Directory.GetFiles(path, f.Item2))
                .OrderBy   (f => f));
            string[] folders = Directory.GetDirectories(path);

            // Parse files and folders into data we can draw!
            activeFiles.Clear();
            activeFiles.AddRange(files.Select(f => new File { 
                type      = FileType.File, 
                extension = Path.GetExtension(f),
                name      = Path.GetFileNameWithoutExtension(f)}));
            activeFiles.AddRange(folders.Select(f=>new File{ 
                type      = FileType.Folder, 
                extension = "",
                name      = Path.GetFileName(f)}));
        }

        public bool Show()
        {
            bool result = false;

            UI.WindowBegin(title, ref windowPose, new Vec2(40,0) *Units.cm2m);
            if (UI.Button("Up"))
                DirectoryUp();
            UI.SameLine();
            UI.Label(path);
            for (int i = 0; i < activeFiles.Count; i++)
            {
                if (UI.Button(activeFiles[i].name+"\n"+activeFiles[i].extension))
                    result = Select(i);
                UI.SameLine();
            }
            UI.WindowEnd();

            return result;
        }
    }
}
