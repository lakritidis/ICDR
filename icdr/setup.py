from distutils.core import setup
from setuptools import find_packages

DESCRIPTION = 'ICDR is a high performing library for retrieving contrastive text data from a document collection.'
LONG_DESCRIPTION = 'ICDR builds an inverted index structure and several fast look-up tables with the aim of '\
    'retrieving similar texts from a corpus. The library is ideal for efficient entity matching, entity resolution, '\
    'record linkage, and deduplication applications in the NLP realm. ICDR allows for very fast retrieval of similar, '\
    'positive (i.e. matching), and negative (i.e. non-matching) text samples which can be used either directly, or to '\
    'fine-tune LLMs and other models.'

setup(
    name='icdr',
    version='0.0.21',
    description=DESCRIPTION,
    long_description=LONG_DESCRIPTION,
    long_description_content_type='text/markdown',
    author="Leonidas Akritidis",
    author_email="lakritidis@ihu.gr",
    maintainer="Leonidas Akritidis",
    maintainer_email="lakritidis@ihu.gr",
    packages=find_packages(),
    url='https://github.com/lakritidis/icdr',
    install_requires=["pandas", "matplotlib"],
    license="Apache",
    keywords=[
        "index", "inverted index", "contrastive data", "pairs", "information retrieval",
        "similarity search", "search", "string search", "approximate retrieval"],
    py_modules=["flagr"],
    package_data={'': ['icdr.so', 'icdr.dylib', 'icdr.dll', 'libgcc_s_seh-1.dll', 'libstdc++-6.dll']}
)
