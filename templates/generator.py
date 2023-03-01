# pylint: disable=missing-function-docstring, missing-module-docstring
import glob
import os
import subprocess

PATH_TO_TEMPLATES = "./"  # must contains trailing slash


# project_path must contains trailing slash
def generator(template_name, project_path, project_name, args):
    full_template_path = PATH_TO_TEMPLATES + template_name
    full_project_path = project_path + project_name

    if not os.path.isdir(full_template_path):
        raise Exception("Can't find template: " + template_name)

    if not os.path.isdir(full_project_path):
        print("Directory [" + full_project_path + "] not exists. Creating...")
        os.makedirs(full_project_path, exist_ok=True)
        print("Directory [" + full_project_path + "] created")

    os.chdir(full_template_path)

    # Get a list of the template files
    template_file_list = glob.glob(full_template_path + "/**", recursive=True)

    os.chdir(full_project_path)
    # working in project directory
    for file in template_file_list:
        # create a directory if needed
        os.makedirs(os.path.dirname(file), exist_ok=True)
        # change first template name to the project name in the file name

        f = os.path.dirname(file) + "/" + os.path.basename(file).replace(template_name, project_name, 1)

        subprocess.run(["sed", "s/APROJ_PROJECT_NAME/" + project_name + "/g", file, " > ", f])
