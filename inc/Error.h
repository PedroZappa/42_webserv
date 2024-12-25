/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/24 10:04:04 by passunca          #+#    #+#             */
/*   Updated: 2024/12/25 21:33:04 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
/// Sauce taken from the great afmyhouse (antoda-s):
/// https://github.com/afmyhouse/4209-minishell/blob/debug/include/error.h

#ifndef ERROR_H
#define ERROR_H

/* ************************************************************************** */
///	ERROR CODES
/* ************************************************************************** */
#ifndef ERROR
#define ERROR 1
#endif
#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef LINE_ERROR
#define LINE_ERROR 50
#endif

#ifndef ARGS_ERROR
#define ARGS_ERROR 52
#endif

#ifndef SYNTAX_ERROR
#define SYNTAX_ERROR 54
#endif
#ifndef COLOR_ERROR
#define COLOR_ERROR 55
#endif
#ifndef CHILD_EXIT
#define CHILD_EXIT 56
#endif
#ifndef SHOW_MSG
#define SHOW_MSG 70
#endif
/* ************************************************************************** */
///	EXIT CODES
/* ************************************************************************** */
#ifndef PERMISSION_DENIED
#define PERMISSION_DENIED 126
#endif
#ifndef COMMAND_NOT_FOUND
#define COMMAND_NOT_FOUND 127
#endif

/* ************************************************************************** */
///	MALLOC ERROR CODES
/* ************************************************************************** */
#ifndef MALLOC_ERROR
#define MALLOC_ERROR 900
#endif
#ifndef MALLOC_NOT_ALLOCATED
#define MALLOC_NOT_ALLOCATED 901
#endif
#ifndef MALLOC_NOT_FREED
#define MALLOC_NOT_FREED 902
#endif
#ifndef MALLOC_NOT_REALLOCATED
#define MALLOC_NOT_REALLOCATED 903
#endif
#ifndef D_MALLOC
#define D_MALLOC 904
#endif
#ifndef D_FREE
#define D_FREE 905
#endif
/* ************************************************************************** */
///	FILE ERROR CODES and STATUS
/* ************************************************************************** */
#ifndef FILE_NOT_FOUND
#define FILE_NOT_FOUND 1000
#endif

#endif
