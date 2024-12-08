/*
		Project:		Crontab
		Module:			cronconst,h
		Description:	some constants
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2024 Martin Gäckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Austria, Linz ``AS IS''
		AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
		TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
		PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR
		CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
		SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
		LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
		USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
		ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
		OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
		OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
		SUCH DAMAGE.
*/

#ifndef CRON_CONST_H
#define CRON_CONST_H

#define IDI_SDIAPPLICATION      1001            // Application icon

#define BUFFER_SIZE		256

#if defined( __WIN32__ )
#define SOFTWARE_KEY "Software"
#elif defined( __WIN64__ )
#define SOFTWARE_KEY "Software\\Wow6432Node"
#else
#error "What windows???"
#endif

#define COMPANY			"Gak"
#define SERVICE_NAME	"GakCronTab"
#define COMMAND_LINE	"commandLine"
#define INTERVAL		"interval"
#define INTERVAL_TYPE	"intervalType"
#define NEXT_START		"nextStart"
#define MULTIPLE_INST	"multipleInstances"

#endif

