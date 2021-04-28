from setuptools import setup
from os.path import join
import ashlang

f = open('README.md', mode='r', encoding='utf8')
long_desc = f.read()
f.close()

setup(
    # Metadata
    name='ashlang',
    version=ashlang.__version__,

    license="MIT",

    author='Damien Gouteux',
    author_email='damien.gouteux@gmail.com',
    url="https://xitog.github.io/dgx/informatique/ash_guide.html",
    maintainer='Damien Gouteux',
    maintainer_email='damien.gouteux@gmail.com',
    
    description='A simple language transpiling to Python',
    long_description=long_desc,
    long_description_content_type="text/markdown",
    # https://pypi.org/classifiers/
    classifiers=[
        'Development Status :: 1 - Planning',
        'License :: OSI Approved :: MIT License',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Programming Language :: Python :: 3 :: Only',
        'Programming Language :: Python :: Implementation :: CPython',
        'Topic :: Software Development',
        'Topic :: Software Development :: Code Generators',
        'Topic :: Software Development :: Compilers',
        'Topic :: Software Development :: Interpreters'
    ],
    keywords=['ash', 'ashlang', 'programming language', 'script', 'scripting', 'transpiler'],
    
    packages=['ashlang'],  #same as name
    python_requires='>=3.5',
    install_requires = ['weyland>=0.1.1'],
)
