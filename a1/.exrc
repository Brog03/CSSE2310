if &cp | set nocp | endif
map  mzggVG:py3f /local/courses/csse2310/bin/clang-format.py'zzz
let s:cpo_save=&cpo
set cpo&vim
vmap gx <Plug>NetrwBrowseXVis
nmap gx <Plug>NetrwBrowseX
vnoremap <silent> <Plug>NetrwBrowseXVis :call netrw#BrowseXVis()
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#BrowseX(expand((exists("g:netrw_gx")? g:netrw_gx : '<cfile>')),netrw#CheckIfRemote())
imap  :py3f /local/courses/csse2310/bin/clang-format.py
let &cpo=s:cpo_save
unlet s:cpo_save
set autoindent
set background=dark
set backspace=indent,eol,start
set expandtab
set fileencodings=ucs-bom,utf-8,latin1
set foldlevelstart=99
set guicursor=n-v-c:block,o:hor50,i-ci:hor15,r-cr:hor30,sm:block,a:blinkon0
set helplang=en
set hlsearch
set mouse=a
set ruler
set shiftwidth=4
set showmatch
set softtabstop=4
set termguicolors
set viminfo='20,\"50
set wildmenu
" vim: set ft=vim :
