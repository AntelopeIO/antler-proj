# pylint: disable=missing-function-docstring, missing-module-docstring
import os
import argparse
from pathlib import Path
from jinja2 import Template

PATH_TO_TEMPLATES = os.path.dirname(os.path.abspath(__file__))


def generator(template_name: str, project_path: Path, project_name: str, macros: dict):
    full_template_path = Path(PATH_TO_TEMPLATES) / template_name
    full_project_path = Path(project_path).absolute() / project_name

    if not os.path.isdir(full_template_path):
        raise Exception("Can't find template: " + template_name)

    if not os.path.isdir(full_project_path):
        print("Directory [" + str(full_project_path) + "] not exists. Creating...")
        os.makedirs(full_project_path, exist_ok=True)
        print("Directory [" + str(full_project_path) + "] created")

    for root, dirs, files in os.walk(full_template_path):
        rel_path = Path(root).relative_to(full_template_path)
        for filename in files:
            src_file = full_template_path.joinpath(rel_path.joinpath(filename))
            # create path to the destination file
            dest_file = full_project_path.joinpath(rel_path.joinpath(filename))
            os.makedirs(os.path.dirname(dest_file), exist_ok=True)

            # change first template name to the project name in the file name
            if template_name in src_file.name:
                f = src_file.name.replace(template_name, project_name, 1)
                dest_file = dest_file.parent.joinpath(f)
            with open(src_file, "r") as template_text:
                template = Template(template_text.read())
                with open(dest_file, "w") as out_text:
                    out_text.write(template.render(macros))
        for dirname in dirs:
            d = full_project_path.joinpath(dirname)
            os.makedirs(d, exist_ok=True)


# return a dictionary with change pairs { MACRO : VALUE }
def get_macros(project_name: str, macro: list) -> dict:

    res: dict = {"APROJ_PROJECT_NAME": project_name}
    if macro is not None:
        for m in macro:
            key, val = m.split("=", 1)
            res[key] = val
    return res


parser = argparse.ArgumentParser(description="Project generator")

parser.add_argument("--project_path", dest="project_path", nargs=1, default="./", help="path to a new project")
parser.add_argument("--project_name", dest="project_name", nargs=1, type=str, required=True, help="name of a new project")
parser.add_argument("--template_name", dest="template_name", nargs=1, type=str, required=True, help="name of a template")
parser.add_argument("-D", nargs="*", dest="macro", help="add macros in format MACRO=VALUE [MACRO=VALUE]")

args = parser.parse_args()

generator(args.template_name[0], args.project_path, args.project_name[0], get_macros(args.project_name[0], args.macro))
