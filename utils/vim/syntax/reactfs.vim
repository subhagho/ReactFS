" Vim syntax file
" Language: Celestia Star Catalogs
" Maintainer: Kevin Lauder
" Latest Revision: 26 April 2008

if exists("b:current_syntax")
  finish
endif

let b:current_syntax = "reactfs"

syn keyword reactfsKeywords token this if while do else 
syn keyword reactfsKeywords for auto sizeof void class public private
syn keyword reactfsKeywords vector unordered_map char string short int long double float bool const override true false std
syn keyword reactfsKeywords list map byte integer boolean timestamp datetime text namespace

highlight link reactfsKeywords Keyword

syntax region reactfsToken start="${"  end="}"
syn keyword reactfsTokenT set_key_fields in on default constraint regex be schema type index typeref
syn keyword reactfsTokenT ASC DESC FULLTEXT HASH TREE

highlight link reactfsToken Type
highlight link reactfsTokenT Type

syntax region reactfsString start=/\v"/ skip=/\v\\./ end=/\v"/
highlight link reactfsString String

" Integer with - + or nothing in front
syn match reactfsNumber '\d\+'
syn match reactfsNumber '[-+]\d\+'

" Floating point number with decimal no E or e 
syn match reactfsNumber '[-+]\d\+\.\d*'

" Floating point like number with E and no decimal point (+,-)
syn match reactfsNumber '[-+]\=\d[[:digit:]]*[eE][\-+]\=\d\+'
syn match reactfsNumber '\d[[:digit:]]*[eE][\-+]\=\d\+'

" Floating point like number with E and decimal point (+,-)
syn match reactfsNumber '[-+]\=\d[[:digit:]]*\.\d*[eE][\-+]\=\d\+'
syn match reactfsNumber '\d[[:digit:]]*\.\d*[eE][\-+]\=\d\+'

syn match reactfsComment '\v//.*$'
" syn region reactfsComment    start="/*"    end="*/"
highlight link reactfsComment Comment

syn match reactfsInclude '#.*$'
highlight link reactfsInclude PreProc

syn match reactfsTemplateName '^.*:$'
highlight link reactfsTemplateName Comment

syn region reactfsCodeBlock start="{" end="}" fold transparent
