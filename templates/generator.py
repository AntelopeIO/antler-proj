# pylint: disable=missing-function-docstring, missing-module-docstring
import glob
import os
import subprocess
import argparse

PATH_TO_TEMPLATES = "./"  # must contains trailing slash


# project_path must contains trailing slash
def generator(template_name: str, project_path: str, project_name: str):
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


parser = argparse.ArgumentParser(description="Project generator")

parser.add_argument("--project_path", dest="project_path", nargs=1, default="./", help="path to a new project")
parser.add_argument("--project_name", dest="project_name", nargs=1, type=str, required=True, help="name of a new project")
parser.add_argument("--template_name", dest="template_name", nargs=1, type=str, required=True, help="name of a template")

args = parser.parse_args()

print(args)

generator(args.template_name[0], args.project_path, args.project_name[0])
